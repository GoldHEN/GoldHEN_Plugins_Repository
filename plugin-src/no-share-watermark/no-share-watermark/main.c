// No Share Block: Removes arbitrary limitations from share feature.
// Authors:
// illusion0001 @ https://github.com/illusion0001
// Nikita Krapivin @ https://github.com/nkrapivin
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include <Common.h>
#include <orbis/Sysmodule.h>
#include <orbis/ScreenShot.h>
#include <orbis/VideoRecording.h>
#include "plugin_common.h"

#define plugin_name "no-share-block"

HOOK_INIT(sceScreenShotSetOverlayImage);
HOOK_INIT(sceScreenShotSetOverlayImageWithOrigin);
HOOK_INIT(sceVideoRecordingSetInfo);
HOOK_INIT(sceScreenShotDisable);

s32 sceScreenShotSetOverlayImage_hook(){
    final_printf("sceScreenShotSetOverlayImage patched\n");
    return 0;
}

s32 sceScreenShotSetOverlayImageWithOrigin_hook(){
    final_printf("sceScreenShotSetOverlayImageWithOrigin patched\n");
    return 0;
}

s32 sceVideoRecordingSetInfo_hook(s32 iInfoType, const void *pInfoData, size_t ulInfoLen) {
    if (iInfoType == 0x000D && pInfoData && sizeof(s32) == ulInfoLen && 0x1 == *(int*)pInfoData) {
        final_printf("sceVideoRecordingSetInfo patched\n");
        // trying to block the recorder, nope...
        return 0; // SCE_OK
    }

    // carry on...
    return HOOK_CONTINUE(sceVideoRecordingSetInfo, int(*)(int, const void*, size_t), iInfoType, pInfoData, ulInfoLen);
}

s32 sceScreenShotDisable_hook(){
    final_printf("sceScreenShotDisable patched\n");
    return 0;
}

s32 attr_module_hidden module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    boot_ver();
    sceSysmoduleLoadModule(0x009c);
    sceSysmoduleLoadModule(0x0085);
    HOOK32(sceScreenShotSetOverlayImage);
    HOOK32(sceScreenShotSetOverlayImageWithOrigin);
    HOOK32(sceVideoRecordingSetInfo);
    HOOK32(sceScreenShotDisable);
    return 0;
}

s32 attr_module_hidden module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    UNHOOK(sceScreenShotSetOverlayImage);
    UNHOOK(sceScreenShotSetOverlayImageWithOrigin);
    UNHOOK(sceVideoRecordingSetInfo);
    UNHOOK(sceScreenShotDisable);
    return 0;
}
