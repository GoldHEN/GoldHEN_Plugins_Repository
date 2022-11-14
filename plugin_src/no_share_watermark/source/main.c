// No Share Block: Removes arbitrary limitations from share feature.
// Authors:
// illusion0001 @ https://github.com/illusion0001
// Nikita Krapivin @ https://github.com/nkrapivin
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <Common.h>
#include <orbis/Sysmodule.h>
#include <orbis/ScreenShot.h>
#include <orbis/VideoRecording.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = "no_share_watermark";
attr_public const char *g_pluginDesc = "Removes arbitrary limitations from share feature in games";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

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

s32 sceVideoRecordingSetInfo_hook(s32 iInfoType, const void *pInfoData, s64 ulInfoLen) {
    if (iInfoType == 0x000D && pInfoData && sizeof(s32) == ulInfoLen && 0x1 == *(s32*)pInfoData) {
        final_printf("sceVideoRecordingSetInfo patched\n");
        // trying to block the recorder, nope...
        return 0; // SCE_OK
    }

    // carry on...
    return HOOK_CONTINUE(sceVideoRecordingSetInfo, s32(*)(s32, const void*, s64), iInfoType, pInfoData, ulInfoLen);
}

s32 sceScreenShotDisable_hook(){
    final_printf("sceScreenShotDisable patched\n");
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    sceSysmoduleLoadModule(0x009c);
    sceSysmoduleLoadModule(0x0085);
    HOOK32(sceScreenShotSetOverlayImage);
    HOOK32(sceScreenShotSetOverlayImageWithOrigin);
    HOOK32(sceVideoRecordingSetInfo);
    HOOK32(sceScreenShotDisable);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceScreenShotSetOverlayImage);
    UNHOOK(sceScreenShotSetOverlayImageWithOrigin);
    UNHOOK(sceVideoRecordingSetInfo);
    UNHOOK(sceScreenShotDisable);
    return 0;
}
