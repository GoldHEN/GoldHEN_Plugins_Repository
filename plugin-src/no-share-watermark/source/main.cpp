// No Share Watermark: Removes watermark from Screenshot using Share button.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include <Common.h>
#include <orbis/Sysmodule.h>
#include <orbis/ScreenShot.h>
#include "../../../common/plugin_common.h"

HOOK_INIT(sceScreenShotSetOverlayImage);
HOOK_INIT(sceScreenShotSetOverlayImageWithOrigin);

s32 sceScreenShotSetOverlayImage_hook(){
    final_printf("sceScreenShotSetOverlayImage patched\n");
    return 0;
}

s32 sceScreenShotSetOverlayImageWithOrigin_hook(){
    final_printf("sceScreenShotSetOverlayImageWithOrigin patched\n");
    return 0;
}

extern "C" {
    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
        final_printf("[GoldHEN] <no-share-watermark> module_start\n");
        boot_ver();
        sceSysmoduleLoadModule(0x009c);
        HOOK(sceScreenShotSetOverlayImage);
        HOOK(sceScreenShotSetOverlayImageWithOrigin);
        return 0;
    }

    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
        final_printf("[GoldHEN] module_stop\n");
        UNHOOK(sceScreenShotSetOverlayImage);
        UNHOOK(sceScreenShotSetOverlayImageWithOrigin);
        return 0;
    }
}
