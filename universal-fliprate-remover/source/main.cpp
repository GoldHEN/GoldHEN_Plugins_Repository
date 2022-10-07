// Fliprate Remover: Universally removes framerate limit for games using
// system library `sceVideoOutSetFlipRate`
// may not be used with games that have fixed timestep.

// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/illusion0001/ps4-plugins

#include <Common.h>
#include <orbis/VideoOut.h>
#include "../../common/plugin_common.h"

HOOK_INIT(sceVideoOutSetFlipRate);

s32 sceVideoOutSetFlipRate_hook(s32 handle, s32 fliprate){
    return 0;
}

extern "C" {
    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
        final_printf("[GoldHEN] module_start\n");
        HOOK32(sceVideoOutSetFlipRate);
        return 0;
    }

    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
        final_printf("[GoldHEN] module_stop\n");
        UNHOOK(sceVideoOutSetFlipRate);
        return 0;
    }
}
