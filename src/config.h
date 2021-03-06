//
// Created by marcsello on 21/01/2021.
//

#ifndef WAKEONLANGATEWAY_CONFIG_H
#define WAKEONLANGATEWAY_CONFIG_H

#include "stdbool.h"

typedef struct {
    unsigned int http_port;
    bool log_requests;
} t_configuration;

t_configuration* load_config(const char* config_path);

void free_config(t_configuration* configuration_ptr);

#endif //WAKEONLANGATEWAY_CONFIG_H
