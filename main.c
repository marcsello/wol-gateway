#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bits/types/sig_atomic_t.h>
#include <signal.h>

#include "wol.h"

#define MESSAGE_OK "OK\n"
#define MESSAGE_NOT_FOUND "NOT FOUND\n"
#define MESSAGE_METHOD_NOT_ALLOWED "METHOD NOT ALLOWED\n"


typedef struct t_request_context {
    size_t total_body_size;
} t_request_context;

typedef struct t_response_store {
    struct MHD_Response *response_method_not_allowed;
    struct MHD_Response *response_not_found;
    struct MHD_Response *response_ok;
} t_response_store;

static int http_handler(void *cls,
                        struct MHD_Connection *connection,
                        const char *url,
                        const char *method,
                        const char *version,
                        const char *upload_data,
                        size_t *upload_data_size,
                        void **ptr) {

    // The first call is required only to create a context
    t_request_context *request_context;
    if (*ptr == NULL) {
        request_context = (t_request_context *) malloc(sizeof(t_request_context));

        if (request_context == NULL) {
            return MHD_NO;
        }

        request_context->total_body_size = 0;
        *ptr = (void *) request_context;
        return MHD_YES;
    } else {
        request_context = (t_request_context *) *ptr;
    }

    // discard the body until it's fully received
    size_t current_size = *upload_data_size;
    if (current_size > 0) {
        request_context->total_body_size += current_size;
        *upload_data_size = 0;
        return MHD_YES;
    }

    // Everything is received, start handling the request
    t_response_store *response_store = (t_response_store *) cls;
    if (request_context->total_body_size) {
        printf("[%s] %s (%ld bytes)\n", method, url, request_context->total_body_size);
    } else {
        printf("[%s] %s\n", method, url);
    }

    // We no longer need the context
    free(request_context);
    *ptr = NULL;

    if (strcmp(url, "/wake") != 0) {
        return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response_store->response_not_found);
    }

    if (strcmp(method, "POST") != 0) {
        return MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response_store->response_method_not_allowed);
    }

    send_wol(); // <- magic happens here

    return MHD_queue_response(connection, MHD_HTTP_OK, response_store->response_ok);
}

int setup_responses(t_response_store **response_store_ptr) {
    t_response_store *response_store = (t_response_store *) malloc(sizeof(t_response_store));

    if (response_store == NULL) {
        return 0;
    }

    // Response OK
    response_store->response_ok =
            MHD_create_response_from_buffer(sizeof(MESSAGE_OK) -1, (void *) MESSAGE_OK, MHD_RESPMEM_PERSISTENT);

    if (response_store->response_ok == NULL) {
        return 0;
    }


    // Response 405
    response_store->response_method_not_allowed =
            MHD_create_response_from_buffer(sizeof(MESSAGE_METHOD_NOT_ALLOWED) -1, (void *) MESSAGE_METHOD_NOT_ALLOWED,
                                            MHD_RESPMEM_PERSISTENT);

    if (response_store->response_method_not_allowed == NULL) {
        return 0;
    }

    // Response 404
    response_store->response_not_found =
            MHD_create_response_from_buffer(sizeof(MESSAGE_NOT_FOUND) -1, (void *) MESSAGE_NOT_FOUND,
                                            MHD_RESPMEM_PERSISTENT);

    if (response_store->response_not_found == NULL) {
        return 0;
    }

    // Add headers
    struct MHD_Response* all_responses[3] = {
            response_store->response_ok,
            response_store->response_method_not_allowed,
            response_store->response_not_found,
    };

    for (int i = 0; i < 3; i++) {
        if (MHD_add_response_header(all_responses[i], "X-Best-Pony", "Derpy") == MHD_NO) {
            return 0;
        }

        if (MHD_add_response_header(all_responses[i], "Content-Type", "text/plain; charset=UTF-8") == MHD_NO) {
            return 0;
        }
    }

    // Set meme
    *response_store_ptr = response_store;

    return 1;
}

void destory_responses(t_response_store *response_store) {

    MHD_destroy_response(response_store->response_ok);
    MHD_destroy_response(response_store->response_not_found);
    MHD_destroy_response(response_store->response_method_not_allowed);
    free(response_store);

}

volatile sig_atomic_t exit_requested = 0;

void signal_handler(int signum) {
    printf("Signal %d received. Exiting...\n", signum);
    exit_requested = 1;
}

int main(int argc, char **argv) {
    printf("Starting WoL Gateway...\n");
    struct MHD_Daemon *d;
    unsigned int flags = MHD_USE_THREAD_PER_CONNECTION |
                         MHD_OPTION_STRICT_FOR_CLIENT |
                         MHD_USE_TCP_FASTOPEN |
                         MHD_USE_INTERNAL_POLLING_THREAD |
                         MHD_USE_DUAL_STACK;

    signal(SIGPIPE, SIG_IGN); // MHD required to ignore sigpipe

    t_response_store *response_store;
    if (!setup_responses(&response_store)) {
        printf("Failed to setup response store");
        return 1;
    }

    d = MHD_start_daemon(flags,
                         8080,
                         NULL, // callback to call to check which clients will be allowed to connect
                         NULL, // extra params for previous
                         &http_handler, // dh = default handler
                         response_store, // args to dh
                         MHD_OPTION_CONNECTION_TIMEOUT,
                         5,
                         MHD_OPTION_END);
    if (d == NULL) {
        printf("Failed to start microhttp server!\n");
        return 1;
    }

    // Register signal handlers for stopping the server
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    printf("WoL Gateway is ready to serve requests!\n");
    do {
        pause();
    } while (!exit_requested);

    // Cleanup
    MHD_stop_daemon(d);
    destory_responses(response_store);
    printf("WoL Gateway stopped.\n");
    return 0;
}