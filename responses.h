//
// Created by marcsello on 21/01/2021.
//

#ifndef WAKEONLANGATEWAY_RESPONSES_H
#define WAKEONLANGATEWAY_RESPONSES_H

#include <microhttpd.h>

typedef struct __attribute__((__packed__)) t_response_store {
    struct MHD_Response *response_internal_server_error;
    struct MHD_Response *response_method_not_allowed;
    struct MHD_Response *response_not_found;
    struct MHD_Response *response_ok;
} t_response_store;

t_response_store *init_responses();

void destory_responses(t_response_store *response_store);

#endif //WAKEONLANGATEWAY_RESPONSES_H
