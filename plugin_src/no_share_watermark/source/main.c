// No Share Block: Removes arbitrary limitations from share feature.
// Authors:
// illusion0001 @ https://github.com/illusion0001
// Nikita Krapivin @ https://github.com/nkrapivin
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <Common.h>
#include <orbis/Sysmodule.h>
#include <orbis/ScreenShot.h>
#include <orbis/VideoRecording.h>
#include <orbis/Remoteplay.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = "no_share_watermark";
attr_public const char *g_pluginDesc = "Removes arbitrary limitations from share feature in games";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00
#define SCE_SYSMODULE_SCREEN_SHOT 0x009c
#define SCE_SYSMODULE_VIDEO_RECORDING 0x0085
#define SCE_SYSMODULE_REMOTE_PLAY 0x00b6

HOOK_INIT(sceScreenShotSetOverlayImage);
HOOK_INIT(sceScreenShotSetOverlayImageWithOrigin);
HOOK_INIT(sceVideoRecordingSetInfo);
HOOK_INIT(sceScreenShotDisable);
HOOK_INIT(sceRemoteplayProhibit);
HOOK_INIT(sceRemoteplayProhibitStreaming);

s32 sceRemoteplayProhibit_hook(void){
    debug_printf("sceRemoteplayProhibit patched\n");
    return 0;
}

s32 sceRemoteplayProhibitStreaming_hook(void){
    debug_printf("sceRemoteplayProhibitStreaming patched\n");
    return 0;
}

s32 sceScreenShotSetOverlayImage_hook(void){
    final_printf("sceScreenShotSetOverlayImage patched\n");
    return 0;
}

s32 sceScreenShotSetOverlayImageWithOrigin_hook(void){
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

s32 sceScreenShotDisable_hook(void){
    final_printf("sceScreenShotDisable patched\n");
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    sceSysmoduleLoadModule(SCE_SYSMODULE_SCREEN_SHOT);
    sceSysmoduleLoadModule(SCE_SYSMODULE_VIDEO_RECORDING);
    sceSysmoduleLoadModule(SCE_SYSMODULE_REMOTE_PLAY);
    HOOK32(sceScreenShotSetOverlayImage);
    HOOK32(sceScreenShotSetOverlayImageWithOrigin);
    HOOK32(sceVideoRecordingSetInfo);
    HOOK32(sceScreenShotDisable);
    HOOK32(sceRemoteplayProhibit);
    HOOK32(sceRemoteplayProhibitStreaming);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceScreenShotSetOverlayImage);
    UNHOOK(sceScreenShotSetOverlayImageWithOrigin);
    UNHOOK(sceVideoRecordingSetInfo);
    UNHOOK(sceScreenShotDisable);
    UNHOOK(sceRemoteplayProhibit);
    UNHOOK(sceRemoteplayProhibitStreaming);
    return 0;
}
