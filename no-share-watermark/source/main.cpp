// Removes watermark from Screenshot using Share button

#include "Common.h"
#include <orbis/Sysmodule.h>
#include <orbis/ScreenShot.h>

HOOK_INIT(sceScreenShotSetOverlayImage);
HOOK_INIT(sceScreenShotSetOverlayImageWithOrigin);

int sceScreenShotSetOverlayImage_hook(){
    return 0;
}

int sceScreenShotSetOverlayImageWithOrigin_hook(){
    return 0;
}

extern "C" {
    int __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
        klog("[GoldHEN] module_start\n");
        uint16_t ScreenshotID = 0x9c;
        sceSysmoduleLoadModule(ScreenshotID);
        HOOK(sceScreenShotSetOverlayImage);
        HOOK(sceScreenShotSetOverlayImageWithOrigin);
        return 0;
    }

    int __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
        klog("[GoldHEN] module_stop\n");
        UNHOOK(sceScreenShotSetOverlayImage);
        UNHOOK(sceScreenShotSetOverlayImageWithOrigin);
        return 0;
    }
}
