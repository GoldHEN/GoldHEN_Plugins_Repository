#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "plugin_common.h"

#define PLUGIN_NAME "plugin_template"


int32_t attr_module_hidden module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] %s Plugin started.\n", PLUGIN_NAME);

    return 0;
}

int32_t attr_module_hidden module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] %s Plugin Ended.\n", PLUGIN_NAME);
    return 0;
}
