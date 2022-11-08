// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "Common.h"
#include "../../../common/plugin_common.h"

const char *plugin_name = "AFR";

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

int sceKernelOpen_hook(const char *path, int flags, OrbisKernelMode mode) {
    debug_printf("path: %s len %li\n", path, strlen(path));

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
        int fd;
        char possible_path[512];

        memset(possible_path, 0, sizeof(possible_path));
        strcpy(possible_path, "/data/GoldHEN/AFR/");
        strcat(possible_path, titleid);
        strcat(possible_path, "/");
        strcat(possible_path, path + 6);

        fd = HOOK_CONTINUE(sceKernelOpen,
                           int (*)(const char *, int, OrbisKernelMode),
                           possible_path, flags, mode);

        debug_printf("possible_path: %s fd: 0x%08x len %li\n", possible_path, fd, strlen(possible_path));
        if (fd >= 0) return fd;
    }

    return HOOK_CONTINUE(sceKernelOpen,
                         int (*)(const char *, int, OrbisKernelMode),
                         path, flags, mode);
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    boot_ver();
    struct proc_info procInfo;

    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        print_proc_info();
    }
    if (procInfo.titleid[0] == 0) {
        final_printf("procInfo.titleid == 0! Assuming %s is system process\n", procInfo.name);
        return 0;
    }
    HOOK32(sceKernelOpen);
    HOOK32(fopen);
    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    UNHOOK(sceKernelOpen);
    UNHOOK(fopen);
    return 0;
}
