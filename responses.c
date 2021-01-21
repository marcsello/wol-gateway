//
// Created by marcsello on 21/01/2021.
//

#include "responses.h"
#include <stdlib.h>


#define MESSAGE_OK "OK\n"
#define MESSAGE_NOT_FOUND "NOT FOUND\n"
#define MESSAGE_METHOD_NOT_ALLOWED "METHOD NOT ALLOWED\n"
#define MESSAGE_INTERNAL_SERVER_ERROR "INTERNAL SERVER ERROR\n"

#define RESPONSE_STORE_MEMBER_COUNT 4

int setup_responses(t_response_store **response_store_ptr) {
    t_response_store *response_store = (t_response_store *) malloc(sizeof(t_response_store));

    if (response_store == NULL) {
        return 0;
    }

    // Response 200
    response_store->response_ok =
            MHD_create_response_from_buffer(sizeof(MESSAGE_OK) - 1, (void *) MESSAGE_OK, MHD_RESPMEM_PERSISTENT);


    // Response 405
    response_store->response_method_not_allowed =
            MHD_create_response_from_buffer(sizeof(MESSAGE_METHOD_NOT_ALLOWED) - 1, (void *) MESSAGE_METHOD_NOT_ALLOWED,
                                            MHD_RESPMEM_PERSISTENT);

    // Response 404
    response_store->response_not_found =
            MHD_create_response_from_buffer(sizeof(MESSAGE_NOT_FOUND) - 1, (void *) MESSAGE_NOT_FOUND,
                                            MHD_RESPMEM_PERSISTENT);

    // Response 500
    response_store->response_internal_server_error =
            MHD_create_response_from_buffer(sizeof(MESSAGE_INTERNAL_SERVER_ERROR) - 1,
                                            (void *) MESSAGE_INTERNAL_SERVER_ERROR,
                                            MHD_RESPMEM_PERSISTENT);

    // Add headers
    struct MHD_Response **response_store_elements = (struct MHD_Response**)response_store;

    for (int i = 0; i < RESPONSE_STORE_MEMBER_COUNT; i++) {

        if (response_store_elements[i] == NULL) {
            return 0;
        }

        if (MHD_add_response_header(response_store_elements[i], "X-Best-Pony", "Derpy") == MHD_NO) {
            return 0;
        }

        if (MHD_add_response_header(response_store_elements[i], "Content-Type", "text/plain; charset=UTF-8") == MHD_NO) {
            return 0;
        }
    }

    // Set meme
    *response_store_ptr = response_store;

    return 1;
}

void destory_responses(t_response_store *response_store) {

    struct MHD_Response **response_store_elements = (struct MHD_Response**)response_store;

    for (int i = 0; i < RESPONSE_STORE_MEMBER_COUNT; i++) {
        MHD_destroy_response(response_store_elements[i]);
    }

    free(response_store);

}
