#include "Common.h"

HOOK_INIT(sceKernelOpen);

char *possible_path;
char titleid[16];

int sceKernelOpen_hook(const char *path, int flags, OrbisKernelMode mode) {
    printf("sceKernelOpen_hook: %s\n", path);

    if (path[0] == '/' && path[1] == 'a' && path[2] == 'p' && path[3] == 'p' &&
        path[4] == '0' && strlen(path) > 6) {
        int fd;

        memset(possible_path, 0, 500);
        strcpy(possible_path, "/data/");
        strcat(possible_path, titleid);
        strcat(possible_path, "/");
        strcat(possible_path, path + 6);

        fd = Detour_sceKernelOpen->Stub<int>(possible_path, flags, mode);

        // printf("path:%s fd:%d\n", possible_path, fd);
        if (fd >= 0) return fd;
    }

    return HOOK_CONTINUE(sceKernelOpen, int, (path, flags, mode));
}

extern "C" {
int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(size_t argc, const void *args) {
    klog("[GoldHEN] module_start\n");

    proc_info procInfo;

    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, 16);

        printf(
            "[+] process info - pid:%d name:%s path:%s titleid:%s contentid:%s "
            "version:%s base_address:0x%lx\n",
            procInfo.pid, procInfo.name, procInfo.path, procInfo.titleid,
            procInfo.contentid, procInfo.version, procInfo.base_address);
    }

    possible_path = new char[500];

    HOOK32(sceKernelOpen);

    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(size_t argc, const void *args) {
    klog("[GoldHEN] module_stop\n");

    UNHOOK(sceKernelOpen);

    delete possible_path;

    return 0;
}
}
