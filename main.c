#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bits/types/sig_atomic_t.h>
#include <signal.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"

typedef struct t_request_context {
    size_t total_body_size;
} t_request_context;

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
        request_context = (t_request_context *)malloc(sizeof(t_request_context));

        if (request_context == NULL) {
            return MHD_NO;
        }

        request_context->total_body_size = 0;
        *ptr = (void*)request_context;
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
    printf("[%s] %s (%ld bytes)\n", method, url, request_context->total_body_size);


    if (strcmp(method, "POST") != 0) {
        // TODO: Return 405
        return MHD_NO;
    }

    const char* auth_header = MHD_lookup_connection_value(connection,MHD_HEADER_KIND,"Authorization");

    if (auth_header == NULL) {
        return MHD_NO;
    }

    const char *page = cls;
    struct MHD_Response *response;
    int ret;

    // We no longer need the context
    free(request_context);
    *ptr = NULL;

    // TODO: predefined response
    // TODO: emit WoL packet

    response = MHD_create_response_from_buffer(strlen(page),
                                               (void *) page,
                                               MHD_RESPMEM_PERSISTENT);

    ret = MHD_add_response_header (response, "X-Best-Pony", "Derpy");

    if (ret != MHD_YES) {
        return MHD_NO;
    }

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
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

    d = MHD_start_daemon(flags,
                         8080,
                         NULL, // callback to call to check which clients will be allowed to connect
                         NULL, // extra params for previous
                         &http_handler, // dh = default handler
                         PAGE, // args to dh
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
    printf("WoL Gateway stopped.\n");
    return 0;
}