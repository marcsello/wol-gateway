//
// Created by marcsello on 21/01/2021.
//

#include "responses.h"
#include <stdlib.h>


#define MESSAGE_OK "OK\n"
#define MESSAGE_NOT_FOUND "NOT FOUND\n"
#define MESSAGE_METHOD_NOT_ALLOWED "METHOD NOT ALLOWED\n"


int setup_responses(t_response_store **response_store_ptr) {
    t_response_store *response_store = (t_response_store *) malloc(sizeof(t_response_store));

    if (response_store == NULL) {
        return 0;
    }

    // Response OK
    response_store->response_ok =
            MHD_create_response_from_buffer(sizeof(MESSAGE_OK) - 1, (void *) MESSAGE_OK, MHD_RESPMEM_PERSISTENT);

    if (response_store->response_ok == NULL) {
        return 0;
    }


    // Response 405
    response_store->response_method_not_allowed =
            MHD_create_response_from_buffer(sizeof(MESSAGE_METHOD_NOT_ALLOWED) - 1, (void *) MESSAGE_METHOD_NOT_ALLOWED,
                                            MHD_RESPMEM_PERSISTENT);

    if (response_store->response_method_not_allowed == NULL) {
        return 0;
    }

    // Response 404
    response_store->response_not_found =
            MHD_create_response_from_buffer(sizeof(MESSAGE_NOT_FOUND) - 1, (void *) MESSAGE_NOT_FOUND,
                                            MHD_RESPMEM_PERSISTENT);

    if (response_store->response_not_found == NULL) {
        return 0;
    }

    // Add headers
    struct MHD_Response *all_responses[3] = {
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
