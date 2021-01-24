//
// Created by marcsello on 21/01/2021.
//

#include <stdlib.h>
#include <string.h>
#include <ini.h>

#include "config.h"

bool str_to_bool(const char *str) {

    if (strncmp(str, "TRUE", 4) == 0 ||
        strncmp(str, "true", 4) == 0 ||
        strncmp(str, "1", 1) == 0) {
        return true;
    } else {
        return false;
    }

}


static int handler(void *user, const char *section, const char *name, const char *value) {
    t_configuration *configuration_ptr = (t_configuration *) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("wakeonlangateway", "http_port")) {
        configuration_ptr->http_port = strtoul(value, NULL, 10);
    } else if (MATCH("wakeonlangateway", "log_requests")) {
        configuration_ptr->log_requests = str_to_bool(value);
    } else {
        return 0;
    }

    return 1;
}

t_configuration* load_config(const char* config_path) {

    t_configuration *configuration_ptr = (t_configuration*)malloc(sizeof(t_configuration));

    if (ini_parse(config_path, handler, configuration_ptr) < 0) {
        free(configuration_ptr);
        return NULL;
    }

    return configuration_ptr;
}


void free_config(t_configuration* configuration_ptr) {
    free(configuration_ptr);
}