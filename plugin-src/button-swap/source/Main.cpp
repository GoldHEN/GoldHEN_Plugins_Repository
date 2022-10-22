// Author: jocover @ https://github.com/jocover
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "Common.h"
#include "../../../common/plugin_common.h"

const char* plugin_name = "button-swap";

int32_t (*sceSystemServiceParamGetInt)(int32_t paramId, int32_t *value);

HOOK_INIT(sceSystemServiceParamGetInt);

int32_t sceSystemServiceParamGetInt_hook(int32_t paramId, int32_t *value) {
    int32_t ret = 0;

    ret = HOOK_CONTINUE(sceSystemServiceParamGetInt, int32_t(*)(int32_t, int32_t*), paramId, value);

    // swap X and O around in Asia PS4

    if (paramId == 1000) {
        *value = !*value;
    }

    return ret;
}

extern "C" {
int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> module_start\n", plugin_name);
    boot_ver();

    int h = 0;

    sys_dynlib_load_prx("libSceSystemService.sprx", &h);

    sys_dynlib_dlsym(h, "sceSystemServiceParamGetInt",
                     &sceSystemServiceParamGetInt);

    HOOK(sceSystemServiceParamGetInt);

    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> module_stop\n", plugin_name);

    UNHOOK(sceSystemServiceParamGetInt);

    return 0;
}
}
