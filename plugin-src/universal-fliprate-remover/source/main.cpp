// Fliprate Remover: Universally removes framerate limit for games using
// system library `sceVideoOutSetFlipRate`
// may not be used with games that have fixed timestep.

// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include <Common.h>
#include <orbis/VideoOut.h>
#include "../../../common/plugin_common.h"

const char* plugin_name = "fliprate-remover";

HOOK_INIT(sceVideoOutSetFlipRate);

s32 sceVideoOutSetFlipRate_hook(int handle, int fliprate){
    debug_printf("handle 0x%08x fliprate 0x%08x\n", handle, fliprate);
    return 0;
}

extern "C" {
    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
        final_printf("[GoldHEN] <%s> module_stop\n", plugin_name);
        boot_ver();
        HOOK32(sceVideoOutSetFlipRate);
        return 0;
    }

    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
        final_printf("[GoldHEN] <%s> module_stop\n", plugin_name);
        UNHOOK(sceVideoOutSetFlipRate);
        return 0;
    }
}
