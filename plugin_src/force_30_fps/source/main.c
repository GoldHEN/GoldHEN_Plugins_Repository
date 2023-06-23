// Force 30 FPS: Force any game to run at 30 FPS
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <Common.h>
#include <orbis/VideoOut.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = "force_30_fps";
attr_public const char *g_pluginDesc = "Force any game to run at 30 FPS";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

HOOK_INIT(sceVideoOutSetFlipRate);

s32 sceVideoOutSetFlipRate_hook(s32 handle, s32 fliprate)
{
    return HOOK_CONTINUE(sceVideoOutSetFlipRate,
                        s32 (*)(s32, s32),
                        handle, 1);
}

s32 attr_public plugin_load(s32 argc, const char* argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    HOOK32(sceVideoOutSetFlipRate);
    return 0;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceVideoOutSetFlipRate);
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    return 0;
}
