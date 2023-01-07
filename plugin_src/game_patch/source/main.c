// Game Patch: Patches game before boot.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include "patch.h"
#include "tiny-json/tiny-json.h"
#include "utils.h"

attr_public const char *g_pluginName = "game_patch";
attr_public const char *g_pluginDesc = "Patches game before boot";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

#define BASE_PATH_PATCH GOLDHEN_PATH "/patches"
#define BASE_PATH_PATCH_SETTINGS BASE_PATH_PATCH "/settings"
#define BASE_PATH_PATCH_JSON BASE_PATH_PATCH "/json"

char titleid[16] = {0};
char game_elf[32] = {0};
char game_prx[MAX_PATH_] = {0};
char game_ver[8] = {0};

s32 get_key_init(void)
{
    return 0;
}

void mkdir_chmod(char* path, OrbisKernelMode mode)
{
    sceKernelMkdir(path, mode);
    sceKernelChmod(path, mode);
    return;
}

void make_folders(void)
{
    // do this every boot in cases where user
    // may create a folder with 511 perms
    mkdir_chmod(GOLDHEN_PATH, 0777);
    mkdir_chmod(BASE_PATH_PATCH, 0777);
    mkdir_chmod(BASE_PATH_PATCH_JSON, 0777);
    mkdir_chmod(BASE_PATH_PATCH_SETTINGS, 0777);
    return;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    make_folders();
    pid = 0;
    struct proc_info procInfo;
    if (sys_sdk_proc_info(&procInfo) == 0) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        memcpy(game_elf, procInfo.name, sizeof(game_elf));
        memcpy(game_ver, procInfo.version, sizeof(game_ver));
        print_proc_info();
    }
    pid = procInfo.pid;
    s32 ret = get_key_init();
    final_printf("get_key_init() = 0x%08x\n", ret);
    return ret;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    return 0;
}
