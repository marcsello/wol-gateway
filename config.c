//
// Created by marcsello on 21/01/2021.
//

#include <stdlib.h>
#include <string.h>
#include <ini.h>

#include "config.h"


static int handler(void *user, const char *section, const char *name, const char *value) {
    t_configuration *configuration_ptr = (t_configuration *) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("wakeonlangateway", "http_port")) {
        configuration_ptr->http_port = strtoul(value, NULL, 10);
    } else {
        return 0;
    }

    return 1;
}

int load_config(const char *config_path, t_configuration *configuration_ptr) {

    if (ini_parse(config_path, handler, configuration_ptr) < 0) {
        return 0;
    }

    return 1;
}