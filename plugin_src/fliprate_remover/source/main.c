// Fliprate Remover: Universally removes framerate limit for games using
// system library `sceVideoOutSetFlipRate`
// may not be used with games that have fixed timestep.

// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include <Common.h>
#include <orbis/VideoOut.h>
#include "plugin_common.h"

#define PLUGIN_NAME "fliprate_remover"

HOOK_INIT(sceVideoOutSetFlipRate);

s32 sceVideoOutSetFlipRate_hook(s32 handle, s32 fliprate){
    debug_printf("handle 0x%08x fliprate 0x%08x\n", handle, fliprate);
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", PLUGIN_NAME, __func__);
    boot_ver();
    HOOK32(sceVideoOutSetFlipRate);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", PLUGIN_NAME, __func__);
    UNHOOK(sceVideoOutSetFlipRate);
    return 0;
}
