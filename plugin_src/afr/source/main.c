// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "Common.h"
#include "plugin_common.h"

#define MAX_PATH_ 260
#define PLUGIN_NAME "afr"

HOOK_INIT(sceKernelOpen);
#if (__FINAL__) == 0
HOOK_INIT(sceKernelStat);
#endif
HOOK_INIT(fopen);

char titleid[16];

FILE* fopen_hook(const char *path, const char *mode) {
    s64 path_len = strlen(path);
    debug_printf("path: %s mode: %s strlen: %li\n", path, mode, path_len);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
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
    s32 ret = stat(path, stat_buf);
    debug_printf("path: %s stat: 0x%08x\n", path, ret);
    return ret;
}

s32 sceKernelOpen_hook(const char *path, s32 flags, OrbisKernelMode mode) {
    s64 path_len = strlen(path);
    debug_printf("path: %s strlen: %li\n", path, path_len);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
        s32 fd;
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

    return HOOK_CONTINUE(sceKernelOpen,
                         s32 (*)(const char *, s32, OrbisKernelMode),
                         path, flags, mode);
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", PLUGIN_NAME, __func__);
    boot_ver();
    struct proc_info procInfo;
    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        print_proc_info();
    }
    HOOK32(sceKernelOpen);
#if (__FINAL__) == 0
    HOOK32(sceKernelStat);
#endif
    HOOK32(fopen);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", PLUGIN_NAME, __func__);
    UNHOOK(sceKernelOpen);
#if (__FINAL__) == 0
    UNHOOK(sceKernelStat);
#endif
    UNHOOK(fopen);
    return 0;
}
