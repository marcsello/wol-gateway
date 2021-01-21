//
// Created by marcsello on 21/01/2021.
//

#ifndef WAKEONLANGATEWAY_CONFIG_H
#define WAKEONLANGATEWAY_CONFIG_H

typedef struct {
    unsigned int http_port;
} t_configuration;

int load_config(const char* config_path, t_configuration* configuration_ptr);

#endif //WAKEONLANGATEWAY_CONFIG_H
