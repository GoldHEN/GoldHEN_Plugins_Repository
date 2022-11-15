// Game Patch: Patches game before boot.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins

#include "patch.h"
#include "tiny-json/tiny-json.h"
#include "utils.h"

#define plugin_name "GamePatch"

#define TEX_ICON "cxml://psnotification/tex_icon_system"
const char *key_patch = "patch";
const char *key_title = "title";
const char *key_app_ver = "app_ver";
const char *key_patch_ver = "patch_ver";
const char *key_name = "name";
const char *key_author = "author";
const char *key_note = "note";
const char *key_app_titleid = "app_titleid";
const char *key_app_elf = "app_elf";
const char *key_app_type = "type";
const char *key_app_addr = "addr";
const char *key_app_value = "value";
const char *key_patch_list = "patch_list";

const char *base_path = "/data/GoldHEN";
const char *base_path_patch = "/data/GoldHEN/patches";
const char *base_path_patch_json = "/data/GoldHEN/patches/json";
const char *base_path_patch_settings = "/data/GoldHEN/patches/settings";

json_t const *json_key_title;
json_t const *json_key_app_ver;
json_t const *json_key_patch_ver;
json_t const *json_key_name;
json_t const *json_key_author;
json_t const *json_key_note;
json_t const *json_key_app_titleid;
json_t const *json_key_app_elf;

char const *title_val;
char const *app_ver_val;
char const *patch_ver_val;
char const *name_val;
char const *author_val;
char const *note_val;
char const *app_titleid_val;
char const *app_elf_val;

char titleid[16] = {0};
char game_elf[32] = {0};
char game_ver[8] = {0};

void get_key_init() {
    u64 patch_lines = 0;
    u64 patch_items = 0;
    char *buffer;
    u64 size;
    char input_file[64];
    snprintf(input_file, sizeof(input_file), "%s/%s.json", base_path_patch_json, titleid);
    int res = Read_File(input_file, &buffer, &size, 32);
    if (res) {
        final_printf("file %s not found\n error: 0x%08x", input_file, res);
        return;
    }
    json_t mem[MAX_TOKENS];
    json_t const *json = json_create(buffer, mem, sizeof mem / sizeof *mem);
    if (!json) {
        Notify(TEX_ICON, "Too many tokens or bad file");
        return;
    }

    json_t const *patchItems = json_getProperty(json, key_patch);
    if (!patchItems || JSON_ARRAY != json_getType(patchItems)) {
        Notify(TEX_ICON, "Patch not found");
        return;
    }

    json_t const *patches;
    for (patches = json_getChild(patchItems); patches != 0; patches = json_getSibling(patches)) {
        if (JSON_OBJ == json_getType(patches)) {
            char const *gameTitle = json_getPropertyValue(patches, key_title);
            if (!gameTitle)
                final_printf("%s: not found\n", key_title);
            char const *gameAppver = json_getPropertyValue(patches, key_app_ver);
            if (!gameAppver)
                final_printf("%s: not found\n", key_app_ver);
            char const *gameAppElf = json_getPropertyValue(patches, key_app_elf);
            if (!gameAppElf)
                final_printf("%s: not found\n", key_app_elf);
            char const *gameName = json_getPropertyValue(patches, key_name);
            if (!gameName)
                final_printf("%s: not found\n", key_name);
            char const *gameAuthor = json_getPropertyValue(patches, key_author);
            if (!gameAuthor)
                final_printf("%s: not found\n", key_author);
            char const *gameNote = json_getPropertyValue(patches, key_note);
            if (!gameNote)
                final_printf("%s: not found\n", key_note);
            json_t const *patch_List_Items = json_getProperty(patches, key_patch_list);
            json_t const *patch_lists;
            u64 hashout = patch_hash_calc(gameTitle, gameName, gameAppver, input_file, gameAppElf);
            char settings_path[64];
            snprintf(settings_path, sizeof(settings_path),
                     "/data/GoldHEN/patches/settings/0x%016lx.txt", hashout);
            char *buffer2;
            u64 size2;
            int res = Read_File(settings_path, &buffer2, &size2, 32);
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
                            char const *gameType = json_getPropertyValue(patch_lists, key_app_type);
                            if (!gameType) {
                                final_printf("  %s: not found\n", key_app_type);
                                return;
                            }
                            char const *gameAddr = json_getPropertyValue(patch_lists, key_app_addr);
                            if (!gameAddr) {
                                final_printf("  %s: not found\n", key_app_addr);
                                return;
                            } else {
                                addr_real = strtoull(gameAddr, NULL, 16);
                            }
                            char const *gameValue =
                                json_getPropertyValue(patch_lists, key_app_value);
                            if (!gameValue) {
                                final_printf("  %s: not found\n", key_app_value);
                                return;
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
    return;
}

void make_folders() {
    mkdir(base_path, 0777);
    mkdir(base_path_patch, 0777);
    mkdir(base_path_patch_json, 0777);
    mkdir(base_path_patch_settings, 0777);
    return;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    boot_ver();
    make_folders();
    pid = 0;
    struct proc_info procInfo;
    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        memcpy(game_elf, procInfo.name, sizeof(game_elf));
        memcpy(game_ver, procInfo.version, sizeof(game_ver));
        print_proc_info();
    }
    pid = procInfo.pid;
    get_key_init();
    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    return 0;
}
