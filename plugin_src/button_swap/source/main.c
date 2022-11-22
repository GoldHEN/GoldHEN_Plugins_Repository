// Author: jocover @ https://github.com/jocover
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include "plugin_common.h"
#include "Common.h"

attr_public const char *g_pluginName = "button_swap";
attr_public const char *g_pluginDesc = "Swap X and O in Asia PS4";
attr_public const char *g_pluginAuth = "jocover";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

s32 (*sceSystemServiceParamGetInt)(s32 paramId, s32 *value);

HOOK_INIT(sceSystemServiceParamGetInt);

s32 sceSystemServiceParamGetInt_hook(s32 paramId, s32 *value) {
    s32 ret = 0;
    ret = HOOK_CONTINUE(sceSystemServiceParamGetInt, s32(*)(s32, s32 *), paramId, value);
    // swap X and O around in Asia PS4
    if (paramId == 1000) {
        *value = !*value;
    }
    return ret;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    s32 h = 0;
    sys_dynlib_load_prx("libSceSystemService.sprx", &h);
    sys_dynlib_dlsym(h, "sceSystemServiceParamGetInt", &sceSystemServiceParamGetInt);
    HOOK32(sceSystemServiceParamGetInt);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceSystemServiceParamGetInt);
    return 0;
}
