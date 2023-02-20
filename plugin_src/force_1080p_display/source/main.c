// force_1080p_display: Reports to the app that it is connected to a 1080p display.

// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <Common.h>
#include <orbis/VideoOut.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = "force_1080p_display";
attr_public const char *g_pluginDesc = "Reports to the app that it is connected to a 1080p display.";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

HOOK_INIT(sceVideoOutGetResolutionStatus);

void print_resStat(OrbisVideoOutResolutionStatus *status)
{
    final_printf("status->width: %u\n", status->width);
    final_printf("status->height: %u\n", status->height);
    final_printf("status->paneWidth: %u\n", status->paneWidth);
    final_printf("status->paneHeight: %u\n", status->paneHeight);
    final_printf("status->refreshRate: %lu (0x%016lx)\n", status->refreshRate, status->refreshRate);
    final_printf("status->screenSize: %f\n", status->screenSize);
    return;
}

s32 sceVideoOutGetResolutionStatus_hook(int handle, OrbisVideoOutResolutionStatus *status)
{
    s32 ret = HOOK_CONTINUE(sceVideoOutGetResolutionStatus,
                           s32 (*)(s32, OrbisVideoOutResolutionStatus*),
                           handle, status);
#if (__FINAL__) == 0
    final_printf("old OrbisVideoOutResolutionStatus *status\n");
    print_resStat(status);
#endif
    status->width = 1920;
    status->height = 1080;
    status->paneWidth = 1920;
    status->paneHeight = 1080;
#if (__FINAL__) == 0
    final_printf("new OrbisVideoOutResolutionStatus *status\n");
    print_resStat(status);
#endif
    return ret;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    HOOK32(sceVideoOutGetResolutionStatus);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceVideoOutGetResolutionStatus);
    return 0;
}
