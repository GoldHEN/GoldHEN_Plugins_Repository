// Author: jocover @ https://github.com/jocover
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "plugin_common.h"
#include "Common.h"

#define plugin_name "button-swap"

int32_t (*sceSystemServiceParamGetInt)(int32_t paramId, int32_t *value);

HOOK_INIT(sceSystemServiceParamGetInt);

int32_t sceSystemServiceParamGetInt_hook(int32_t paramId, int32_t *value) {
    int32_t ret = 0;
    ret = HOOK_CONTINUE(sceSystemServiceParamGetInt, int32_t(*)(int32_t, int32_t *), paramId, value);
    // swap X and O around in Asia PS4
    if (paramId == 1000) {
        *value = !*value;
    }
    return ret;
}

int attr_module_hidden module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    boot_ver();
    int h = 0;
    sys_dynlib_load_prx("libSceSystemService.sprx", &h);
    sys_dynlib_dlsym(h, "sceSystemServiceParamGetInt", &sceSystemServiceParamGetInt);
    HOOK32(sceSystemServiceParamGetInt);
    return 0;
}

int attr_module_hidden module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    UNHOOK(sceSystemServiceParamGetInt);
    return 0;
}
