// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include "Common.h"
#include "plugin_common.h"

#define MAX_PATH_ 260
attr_public const char *g_pluginName = "afr";
attr_public const char *g_pluginDesc = "Application File Redirector";
attr_public const char *g_pluginAuth = "jocover, SiSTR0";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

HOOK_INIT(sceKernelOpen);
HOOK_INIT(sceKernelStat);
HOOK_INIT(fopen);

char titleid[16];

FILE* fopen_hook(const char *path, const char *mode) {
    s64 path_len = strlen(path);
    debug_printf("path: %s mode: %s strlen: %li\n", path, mode, path_len);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && path_len > 6) {
        FILE* fp=NULL;
        char possible_path[MAX_PATH_];

        memset(possible_path, 0, sizeof(possible_path));
        snprintf(possible_path, sizeof(possible_path), "/data/GoldHEN/AFR/%s/%s", titleid, path + 6);

        fp = HOOK_CONTINUE(fopen,
                           FILE *(*)(const char *, const char *),
                           possible_path, mode);
        s64 possible_path_len = strlen(possible_path);
        debug_printf("possible_path: %s strlen: %li FILE*: 0x%p\n", possible_path, possible_path_len, &fp);
        if (fp) return fp;
    }

        return HOOK_CONTINUE(fopen,
                           FILE *(*)(const char *, const char *),
                           path, mode);
}

s32 sceKernelStat_hook(char *path, struct stat* stat_buf) {
    s64 path_len = strlen(path);
    s32 ret = stat(path, stat_buf);
    debug_printf("path: %s stat: 0x%08x\n", path, ret);
    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && path_len > 6 ) {
        char possible_path[MAX_PATH_];
        memset(possible_path, 0, sizeof(possible_path));
        snprintf(possible_path, sizeof(possible_path), "/data/GoldHEN/AFR/%s/%s", titleid, path + 6);
        s32 ret_pos = stat(possible_path, stat_buf);
        debug_printf("possible_path: %s stat: 0x%08x\n", possible_path, ret_pos);
        if (ret_pos < 0) return ret;
        return ret_pos;
      }
    return ret;
}

s32 sceKernelOpen_hook(const char *path, s32 flags, OrbisKernelMode mode) {
    s32 fd = 0;
    s64 path_len = strlen(path);
    debug_printf("path: %s strlen: %li\n", path, path_len);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && path_len > 6) {

        char possible_path[MAX_PATH_];
        memset(possible_path, 0, sizeof(possible_path));
        snprintf(possible_path, sizeof(possible_path), "/data/GoldHEN/AFR/%s/%s", titleid, path + 6);
        s64 possible_path_len = strlen(possible_path);
        fd = HOOK_CONTINUE(sceKernelOpen,
                           s32 (*)(const char *, s32, OrbisKernelMode),
                           possible_path, flags, mode);

        debug_printf("possible_path: %s fd: 0x%08x strlen: %li\n", possible_path, fd, possible_path_len);
        if (fd >= 0) return fd;
    }

    fd = HOOK_CONTINUE(sceKernelOpen,
                       s32 (*)(const char *, s32, OrbisKernelMode),
                       path, flags, mode);
    debug_printf("fd: 0x%08x\n", fd);
    return fd;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
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

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceKernelOpen);
    UNHOOK(sceKernelStat);
    UNHOOK(fopen);
    return 0;
}
