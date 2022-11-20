// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "Common.h"
#include "plugin_common.h"

#define PLUGIN_NAME "AFR"

HOOK_INIT(sceKernelOpen);
HOOK_INIT(fopen);

char titleid[16];

FILE* fopen_hook(const char *path, const char *mode) {
    debug_printf("path: %s len %li\n", path, strlen(path));
    debug_printf("mode: %s\n", mode);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
        FILE* fp=NULL;
        char possible_path[512];

        memset(possible_path, 0, sizeof(possible_path));
        strcpy(possible_path, "/data/GoldHEN/AFR/");
        strcat(possible_path, titleid);
        strcat(possible_path, "/");
        strcat(possible_path, path + 6);

        fp = HOOK_CONTINUE(fopen,
                           FILE *(*)(const char *, const char *),
                           possible_path, mode);

        debug_printf("possible_path: %s len %li\n", possible_path, strlen(possible_path));
        if (fp) return fp;
    }

        return HOOK_CONTINUE(fopen,
                           FILE *(*)(const char *, const char *),
                           path, mode);
}

s32 sceKernelOpen_hook(const char *path, s32 flags, OrbisKernelMode mode) {
    debug_printf("path: %s len %li\n", path, strlen(path));

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
        s32 fd;
        char possible_path[512];

        memset(possible_path, 0, sizeof(possible_path));
        strcpy(possible_path, "/data/GoldHEN/AFR/");
        strcat(possible_path, titleid);
        strcat(possible_path, "/");
        strcat(possible_path, path + 6);

        fd = HOOK_CONTINUE(sceKernelOpen,
                           s32 (*)(const char *, s32, OrbisKernelMode),
                           possible_path, flags, mode);

        debug_printf("possible_path: %s fd: 0x%08x len %li\n", possible_path, fd, strlen(possible_path));
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
    HOOK32(fopen);
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", PLUGIN_NAME, __func__);
    UNHOOK(sceKernelOpen);
    UNHOOK(fopen);
    return 0;
}
