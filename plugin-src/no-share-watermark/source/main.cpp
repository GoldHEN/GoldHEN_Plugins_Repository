// No Share Watermark: Removes watermark from Screenshot using Share button.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include <Common.h>
#include <orbis/Sysmodule.h>
#include <orbis/ScreenShot.h>
#include <orbis/VideoRecording.h>
#include "../../../common/plugin_common.h"

const char* plugin_name = "no-share-watermark";

HOOK_INIT(sceScreenShotSetOverlayImage);
HOOK_INIT(sceScreenShotSetOverlayImageWithOrigin);
HOOK_INIT(sceVideoRecordingSetInfo);

s32 sceScreenShotSetOverlayImage_hook(){
    final_printf("sceScreenShotSetOverlayImage patched\n");
    return 0;
}

s32 sceScreenShotSetOverlayImageWithOrigin_hook(){
    final_printf("sceScreenShotSetOverlayImageWithOrigin patched\n");
    return 0;
}

int sceVideoRecordingSetInfo_hook(int iInfoType, const void *pInfoData, size_t ulInfoLen) {
    if (iInfoType == 0x000D && pInfoData && sizeof(int) == ulInfoLen && 0x1 == *(int*)pInfoData) {
        // trying to block the recorder, nope...
        return 0; // SCE_OK
    }

	// carry on...
    return HOOK_CONTINUE(sceVideoRecordingSetInfo, int(*)(int, const void*, size_t), iInfoType, pInfoData, ulInfoLen);
}

extern "C" {
    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
        final_printf("[GoldHEN] <%s> module_start\n", plugin_name);
        boot_ver();
        sceSysmoduleLoadModule(0x009c);
		sceSysmoduleLoadModule(0x0085);
        HOOK(sceScreenShotSetOverlayImage);
        HOOK(sceScreenShotSetOverlayImageWithOrigin);
		HOOK(sceVideoRecordingSetInfo);
        return 0;
    }

    s32 __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
        final_printf("[GoldHEN] <%s> module_stop\n", plugin_name);
        UNHOOK(sceScreenShotSetOverlayImage);
        UNHOOK(sceScreenShotSetOverlayImageWithOrigin);
		UNHOOK(sceVideoRecordingSetInfo);
        return 0;
    }
}
