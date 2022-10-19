// Author: jocover @ https://github.com/jocover
// Author: SiSTR0 @ https://github.com/SiSTR0
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "Common.h"
#include "../../../common/plugin_common.h"

const char* plugin_name = "AFR";

HOOK_INIT(sceKernelOpen);

char *possible_path;
char titleid[16];
const u32 max_path = 512;

int sceKernelOpen_hook(const char *path, int flags, OrbisKernelMode mode) {
    debug_printf("path: %s\n", path);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
        int fd;

        memset(possible_path, 0, max_path);
        strcpy(possible_path, "/data/GoldHEN/AFR/");
        strcat(possible_path, titleid);
        strcat(possible_path, "/");
        strcat(possible_path, path + 6);

        fd = Detour_sceKernelOpen->Stub<int>(possible_path, flags, mode);

        debug_printf("possible_path: %s fd: %i\n", possible_path, fd);
        if (fd >= 0) return fd;
    }

    return HOOK_CONTINUE(sceKernelOpen, int, (path, flags, mode));
}

extern "C" {
int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> module_start\n", plugin_name);
    boot_ver();
    proc_info procInfo;

    if (!sys_sdk_proc_info(&procInfo)){
        memcpy(titleid, procInfo.titleid, 16);
        print_proc_info();
    }

    possible_path = new char[max_path];

    HOOK32(sceKernelOpen);

    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> module_stop\n", plugin_name);

    UNHOOK(sceKernelOpen);

    delete possible_path;

    return 0;
}
}
