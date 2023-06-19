// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include "Common.h"
#include "plugin_common.h"
#include <fcntl.h>
#include <string.h>

attr_public const char *g_pluginName = "afr";
attr_public const char *g_pluginDesc = "Application File Redirector";
attr_public const char *g_pluginAuth = "jocover, SiSTR0";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

#define APP0_PATH "/app0/"

HOOK_INIT(sceKernelStat);
HOOK_INIT(open);

char titleid[16] = {0};

s32 sceKernelStat_hook(char *path, struct stat* stat_buf)
{
    // FIXME: use errno for correct `stat()` return values
    s32 ret = 0;
    s32 ret_pos = 0;
    ret = stat(path, stat_buf);
    if (!startsWith(path, APP0_PATH) && strlen(path) > strlen(APP0_PATH))
    {
        char possible_path[MAX_PATH_] = {0};
        snprintf(possible_path, sizeof(possible_path), GOLDHEN_PATH "/AFR/%s/%s", titleid, path + strlen(APP0_PATH));

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

// https://man.freebsd.org/cgi/man.cgi?query=open&apropos=0&sektion=2&manpath=FreeBSD+9.0-RELEASE&arch=default&format=html
s32 open_hook(const char *path, s32 flags)
{
    s32 fd = 0;
    debug_printf("input path: %s\n", path);
    debug_printf("input flags: 0x%08x\n", flags);
    if (!startsWith(path, APP0_PATH) && strlen(path) > strlen(APP0_PATH))
    {
        char possible_path[MAX_PATH_] = {0};
        snprintf(possible_path, sizeof(possible_path), GOLDHEN_PATH "/AFR/%s/%s", titleid, path + strlen(APP0_PATH));
        fd = HOOK_CONTINUE(open,
                           s32 (*)(const char *, s32),
                           possible_path, flags);

        if (fd >= 0)
        {
            final_printf("new_path: %s\n", possible_path);
            final_printf("new fd: 0x%08x\n", fd);
            return fd;
        }
    }

    fd = HOOK_CONTINUE(open,
                       s32 (*)(const char *, s32),
                       path, flags);
    debug_printf("fd: 0x%08x\n", fd);
    return fd;
}

s32 attr_public plugin_load(s32 argc, const char* argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    struct proc_info procInfo;
    if (sys_sdk_proc_info(&procInfo) == 0)
    {
        strncpy(titleid, procInfo.titleid, sizeof(titleid));
        print_proc_info();
        HOOK32(sceKernelStat);
        HOOK32(open);
    }
    else
    {
        final_printf("Unable to get process info from sys_sdk_proc_info");
        return -1;
    }
    return 0;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[])
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceKernelStat);
    UNHOOK(open);
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
