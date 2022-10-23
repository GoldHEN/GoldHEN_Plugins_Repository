// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "Common.h"
#include "../../../common/plugin_common.h"

const char *plugin_name = "AFR";

HOOK_INIT(sceKernelOpen);

char titleid[16];

int sceKernelOpen_hook(const char *path, int flags, OrbisKernelMode mode) {
    debug_printf("path: %s\n", path);

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

        debug_printf("possible_path: %s fd: 0x%08x\n", possible_path, fd);
        if (fd >= 0) return fd;
    }

    return HOOK_CONTINUE(sceKernelOpen,
                         int (*)(const char *, int, OrbisKernelMode),
                         path, flags, mode);
}

extern "C" {
int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> module_start\n", plugin_name);
    boot_ver();
    proc_info procInfo;

    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        print_proc_info();
    }
    if (procInfo.titleid[0] == 0) {
        final_printf("procInfo.titleid == 0! Assuming %s is system process\n", procInfo.name);
        return 0;
    }
    HOOK32(sceKernelOpen);
    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> module_stop\n", plugin_name);

    UNHOOK(sceKernelOpen);

    return 0;
}
}
