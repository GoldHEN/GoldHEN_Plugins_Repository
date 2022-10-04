// Universally removes framerate limit for games using
// system library `sceVideoOutSetFlipRate`
// may not be used with games that have fixed timestep

#include "Common.h"
#include <orbis/VideoOut.h>

HOOK_INIT(sceVideoOutSetFlipRate);

int sceVideoOutSetFlipRate_hook(int handle, int fliprate){
// printf("sceVideoOutSetFlipRate(int handle = 0x%08x, int fliprate = 0x%08x);\n", handle, fliprate);
    return 0;
}

extern "C" {
    int __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
        klog("[GoldHEN] module_start\n");
        HOOK32(sceVideoOutSetFlipRate);
        return 0;
    }

    int __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
        klog("[GoldHEN] module_stop\n");
        UNHOOK(sceVideoOutSetFlipRate);
        return 0;
    }
}
