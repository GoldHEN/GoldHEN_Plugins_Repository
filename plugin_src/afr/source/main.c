// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include "Common.h"
#include "plugin_common.h"

attr_public const char *g_pluginName = "afr";
attr_public const char *g_pluginDesc = "Application File Redirector";
attr_public const char *g_pluginAuth = "jocover, SiSTR0";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

HOOK_INIT(sceKernelOpen);
HOOK_INIT(sceKernelStat);
HOOK_INIT(fopen);

char titleid[16];

FILE* fopen_hook(const char *path, const char *mode)
{
    FILE* fp = NULL;
    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6)
    {
        char possible_path[MAX_PATH_];
        memset(possible_path, 0, sizeof(possible_path));
        snprintf(possible_path, sizeof(possible_path), GOLDHEN_PATH "/AFR/%s/%s", titleid, path + 6);

        fp = HOOK_CONTINUE(fopen,
                           FILE *(*)(const char *, const char *),
                           possible_path, mode);
        if (fp)
        {
            final_printf("new_path: %s FILE*: 0x%p\n", possible_path, &fp);
            return fp;
        }
    }

    fp = HOOK_CONTINUE(fopen,
                       FILE *(*)(const char *, const char *),
                       path, mode);
    debug_printf("path: %s FILE*: 0x%p\n", path, &fp);
    return fp;
}

s32 sceKernelStat_hook(char *path, struct stat* stat_buf)
{
    // FIXME: use errno for correct `stat()` return values
    s32 ret = 0;
    s32 ret_pos = 0;
    ret = stat(path, stat_buf);
    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6 )
    {
        char possible_path[MAX_PATH_];
        memset(possible_path, 0, sizeof(possible_path));
        snprintf(possible_path, sizeof(possible_path), GOLDHEN_PATH "/AFR/%s/%s", titleid, path + 6);

        ret_pos = stat(possible_path, stat_buf);
        if (ret_pos < 0)
        {
            debug_printf("old: %s\n", path);
            debug_printf("old stat: 0x%08x\n", ret);
            return ret;
        }
        else
        {
            final_printf("new: %s\n", possible_path);
            final_printf("new stat: 0x%08x\n", ret_pos);
            return ret_pos;
        }
    }
    return ret;
}

s32 sceKernelOpen_hook(const char *path, s32 flags, OrbisKernelMode mode)
{
    s32 fd = 0;
    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {

        char possible_path[MAX_PATH_];
        memset(possible_path, 0, sizeof(possible_path));
        snprintf(possible_path, sizeof(possible_path), GOLDHEN_PATH "/AFR/%s/%s", titleid, path + 6);
        fd = HOOK_CONTINUE(sceKernelOpen,
                           s32 (*)(const char *, s32, OrbisKernelMode),
                           possible_path, flags, mode);

        if (fd >= 0)
        {
            final_printf("new_path: %s\n", possible_path);
            final_printf("new fd: 0x%08x\n", fd);
            return fd;
        }
    }

    fd = HOOK_CONTINUE(sceKernelOpen,
                       s32 (*)(const char *, s32, OrbisKernelMode),
                       path, flags, mode);
    debug_printf("path: %s\n", path);
    debug_printf("fd: 0x%08x\n", fd);
    return fd;
}

s32 attr_public plugin_load(s32 argc, const char* argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    struct proc_info procInfo;
    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        print_proc_info();
    }
    HOOK32(sceKernelOpen);
    HOOK32(sceKernelStat);
    HOOK32(fopen);
    return 0;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceKernelOpen);
    UNHOOK(sceKernelStat);
    UNHOOK(fopen);
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
