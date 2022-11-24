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

#define KEY_PATCH "patch"
#define KEY_TITLE "title"
#define KEY_APP_VER "app_ver"
#define KEY_PATCH_VER "patch_ver"
#define KEY_NAME "name"
#define KEY_AUTHOR "author"
#define KEY_NOTE "note"
#define KEY_APP_TITLEID "app_titleid"
#define KEY_APP_ELF "app_elf"
#define KEY_APP_TYPE "type"
#define KEY_APP_ADDR "addr"
#define KEY_APP_VALUE "value"
#define KEY_PATCH_LIST "patch_list"

#define BASE_PATH_PATCH GOLDHEN_PATH "/patches"
#define BASE_PATH_PATCH_SETTINGS BASE_PATH_PATCH "/settings"
#define BASE_PATH_PATCH_JSON BASE_PATH_PATCH "/json"

char titleid[16] = {0};
char game_elf[32] = {0};
char game_prx[MAX_PATH_] = {0};
char game_ver[8] = {0};

s32 get_key_init() {
    u64 patch_lines = 0;
    u64 patch_items = 0;
    char *buffer;
    u64 size;
    char input_file[64];
    snprintf(input_file, sizeof(input_file), "%s/%s.json", BASE_PATH_PATCH_JSON, titleid);
    s32 res = Read_File(input_file, &buffer, &size, 0);

    if (res) {
        final_printf("file %s not found\n error: 0x%08x", input_file, res);
        return -1;
    }

    json_t mem[MAX_TOKENS];
    json_t const *json = json_create(buffer, mem, MAX_TOKENS);
    if (!json) {
        Notify(TEX_ICON, "Too many tokens or bad file");
        return -1;
    }

    json_t const *patchItems = json_getProperty(json, KEY_PATCH);
    if (!patchItems || JSON_ARRAY != json_getType(patchItems)) {
        Notify(TEX_ICON, "Patch not found");
        return -1;
    }

    json_t const *patches;
    for (patches = json_getChild(patchItems); patches != 0; patches = json_getSibling(patches)) {
        if (JSON_OBJ == json_getType(patches)) {
            char const *gameTitle = json_getPropertyValue(patches, KEY_TITLE);
            if (!gameTitle)
                final_printf("%s: not found\n", KEY_TITLE);
            char const *gameAppver = json_getPropertyValue(patches, KEY_APP_VER);
            if (!gameAppver)
                final_printf("%s: not found\n", KEY_APP_VER);
            char const *gameAppElf = json_getPropertyValue(patches, KEY_APP_ELF);
            if (!gameAppElf)
                final_printf("%s: not found\n", KEY_APP_ELF);
            char const *gameName = json_getPropertyValue(patches, KEY_NAME);
            if (!gameName)
                final_printf("%s: not found\n", KEY_NAME);
            char const *gameAuthor = json_getPropertyValue(patches, KEY_AUTHOR);
            if (!gameAuthor)
                final_printf("%s: not found\n", KEY_AUTHOR);
            char const *gameNote = json_getPropertyValue(patches, KEY_NOTE);
            if (!gameNote)
                final_printf("%s: not found\n", KEY_NOTE);
            json_t const *patch_List_Items = json_getProperty(patches, KEY_PATCH_LIST);
            json_t const *patch_lists;
            u64 hashout = patch_hash_calc(gameTitle, gameName, gameAppver, input_file, gameAppElf);
            char settings_path[64];
            snprintf(settings_path, sizeof(settings_path),
                     "/data/GoldHEN/patches/settings/0x%016lx.txt", hashout);
            char *buffer2;
            u64 size2;
            s32 res = Read_File(settings_path, &buffer2, &size2, 0);
            if (res == 0x80020002) {
                final_printf("file %s not found, initializing false. ret: 0x%08x\n", settings_path,
                             res);
                unsigned char false_data[2] = {0x30, 0xa};
                Write_File(settings_path, false_data, sizeof(false_data));
            } else if (res == 0) {
                if (buffer2[0] == 0x31 && strcmp(game_elf, gameAppElf) == 0 &&
                    strcmp(game_ver, gameAppver) == 0) {
                    debug_printf("settings path: %s ret: 0x%08x\n", settings_path, res);
                    patch_items++;
                    for (patch_lists = json_getChild(patch_List_Items); patch_lists != 0;
                         patch_lists = json_getSibling(patch_lists)) {
                        if (JSON_OBJ == json_getType(patch_lists)) {
                            u64 addr_real = 0;
                            char const *gameType = json_getPropertyValue(patch_lists, KEY_APP_TYPE);
                            if (!gameType) {
                                final_printf("  %s: not found\n", KEY_APP_TYPE);
                                return -1;
                            }
                            char const *gameAddr = json_getPropertyValue(patch_lists, KEY_APP_ADDR);
                            if (!gameAddr) {
                                final_printf("  %s: not found\n", KEY_APP_ADDR);
                                return -1;
                            } else {
                                addr_real = strtoull(gameAddr, NULL, 16);
                            }
                            char const *gameValue =
                                json_getPropertyValue(patch_lists, KEY_APP_VALUE);
                            if (!gameValue) {
                                final_printf("  %s: not found\n", KEY_APP_VALUE);
                                return -1;
                            }
                            debug_printf(
                                "game_ver: %s game_elf: %s gameAppElf: %s gameAppver: %s\n",
                                game_ver, game_elf, gameAppElf, gameAppver);
                            debug_printf("setting %s true\n", settings_path);
                            patch_data1(gameType, addr_real, gameValue);
                            patch_lines++;
                        }
                    }
                }
            }
        }
    }
    if (patch_lines > 0) {
        Notify(TEX_ICON, "%li Patches Applied\n"
               "%li Patch Lines Applied",
               patch_items, patch_lines);
    }
    return 0;
}

void make_folders() {
    mkdir(GOLDHEN_PATH, 0777);
    mkdir(BASE_PATH_PATCH, 0777);
    mkdir(BASE_PATH_PATCH_JSON, 0777);
    mkdir(BASE_PATH_PATCH_SETTINGS, 0777);
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
