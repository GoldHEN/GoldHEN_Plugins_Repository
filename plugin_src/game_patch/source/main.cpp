// Game Patch: Patches game before boot.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <mxml.h>
#include "patch.h"
#include "utils.h"

attr_public const char *g_pluginName = "game_patch";
attr_public const char *g_pluginDesc = "Patches game before boot";
attr_public const char *g_pluginAuth = "illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

#define GOLDHEN_PATH_ (char*) GOLDHEN_PATH
#define BASE_PATH_PATCH (char*) GOLDHEN_PATH_ "/patches"
#define BASE_PATH_PATCH_SETTINGS (char*) BASE_PATH_PATCH "/settings"
#define BASE_PATH_PATCH_XML (char*) BASE_PATH_PATCH "/xml"

char titleid[16] = {0};
char game_elf[32] = {0};
char game_prx[MAX_PATH_] = {0};
char game_ver[8] = {0};

const char* GetXMLAttr(mxml_node_t *node, const char *name)
{
    const char* AttrData = mxmlElementGetAttr(node, name);
    if (AttrData == NULL) AttrData = "";
    return AttrData;
}

void get_key_init(void)
{
    u64 patch_lines = 0;
    u64 patch_items = 0;
    char *buffer;
    char *buffer2;
    u64 size = 0;
    u64 size2 = 0;
    char input_file[64];
    snprintf(input_file, sizeof(input_file), BASE_PATH_PATCH_XML "/%s.xml", titleid);
    s32 res = Read_File(input_file, &buffer, &size, 0);

    if (res) {
        final_printf("file %s not found\nerror: 0x%08x", input_file, res);
        return;
    }

    if (buffer) {
        mxml_node_t *node, *tree, *Patchlist_node, *Line_node = NULL;
        tree = mxmlLoadString(NULL, buffer, MXML_NO_CALLBACK);

        if (!tree) {
            final_printf("XML: could not parse XML:\n%s\n", buffer);
            free(buffer);
            return;
        }

        for (node = mxmlFindElement(tree, tree, "Metadata", NULL, NULL, MXML_DESCEND); node != NULL;
             node = mxmlFindElement(node, tree, "Metadata", NULL, NULL, MXML_DESCEND)) {
            const char *TitleData = GetXMLAttr(node, "Title");
            const char *NameData = GetXMLAttr(node, "Name");
            const char *AppVerData = GetXMLAttr(node, "AppVer");
            const char *AppElfData = GetXMLAttr(node, "AppElf");

            debug_printf("Title: \"%s\"\n", TitleData);
            debug_printf("Name: \"%s\"\n", NameData);
            debug_printf("AppVer: \"%s\"\n", AppVerData);
            debug_printf("AppElf: \"%s\"\n", AppElfData);

            u64 hashout = patch_hash_calc(TitleData, NameData, AppVerData, input_file, AppElfData);
            char settings_path[64];
            snprintf(settings_path, sizeof(settings_path), BASE_PATH_PATCH_SETTINGS "/0x%016lx.txt", hashout);
            final_printf("Settings path: %s\n", settings_path);
            s32 res = Read_File(settings_path, &buffer2, &size2, 0);
            if (res == ORBIS_KERNEL_ERROR_ENOENT) {
                debug_printf("file %s not found, initializing false. ret: 0x%08x\n", settings_path, res);
                u8 false_data[2] = {'0', '\n'};
                Write_File(settings_path, false_data, sizeof(false_data));
            } else if (buffer2[0] == '1' && !strcmp(game_ver, AppVerData) && !strcmp(game_elf, AppElfData)) {
                patch_items++;
                Patchlist_node = mxmlFindElement(node, node, "PatchList", NULL, NULL, MXML_DESCEND);
                for (Line_node = mxmlFindElement(node, node, "Line", NULL, NULL, MXML_DESCEND); Line_node != NULL;
                     Line_node = mxmlFindElement(Line_node, Patchlist_node, "Line", NULL, NULL, MXML_DESCEND))
                {
                    u64 addr_real = 0;
                    const char *gameType = GetXMLAttr(Line_node, "Type");
                    const char *gameAddr = GetXMLAttr(Line_node, "Address");
                    const char *gameValue = GetXMLAttr(Line_node, "Value");
                    debug_printf("Type: \"%s\"\n", gameType);
                    if (gameAddr)
                    {
                        addr_real = strtoull(gameAddr, NULL, 16);
                        debug_printf("Address: 0x%lx\n", addr_real);
                    }
                    debug_printf("Value: \"%s\"\n", gameValue);
                    debug_printf("patch line: %lu\n", patch_lines);
                    if (gameType && addr_real) // type and address must be present
                    {
                        patch_data1(djb2_hash(gameType), addr_real, gameValue);
                        patch_lines++;
                    }
                }
            }
        }

        mxmlDelete(Line_node);
        mxmlDelete(Patchlist_node);
        mxmlDelete(node);
        mxmlDelete(tree);
        free(buffer);

        if (patch_items > 0 && patch_lines > 0)
        {
            char line_msg[64];
            char item_msg[64];

            if (patch_lines == 1)
                snprintf(line_msg, sizeof(line_msg), "%lu Patch Line Applied", patch_lines);
            else if (patch_lines > 1)
                snprintf(line_msg, sizeof(line_msg), "%lu Patch Lines Applied", patch_lines);
            if (patch_items == 1)
                snprintf(item_msg, sizeof(item_msg), "%lu Patch Applied", patch_items);
            else if (patch_items > 1)
                snprintf(item_msg, sizeof(item_msg), "%lu Patches Applied", patch_items);

            char msg[128];
            snprintf(msg, sizeof(msg),
                     "%s\n"
                     "%s",
                     item_msg, line_msg);
            NotifyStatic(TEX_ICON_SYSTEM, msg);
        }
    }
    else // if (buffer)
    {
        char msg[128];
        snprintf(msg, sizeof(msg), "File %s\nis empty", input_file);
        NotifyStatic(TEX_ICON_SYSTEM, msg);
    }
    return;
}

void mkdir_chmod(const char *path, OrbisKernelMode mode) {
    sceKernelMkdir(path, mode);
    sceKernelChmod(path, mode);
    return;
}

void make_folders(void) {
    mkdir_chmod(GOLDHEN_PATH_, 0777);
    mkdir_chmod(BASE_PATH_PATCH, 0777);
    mkdir_chmod(BASE_PATH_PATCH_XML, 0777);
    mkdir_chmod(BASE_PATH_PATCH_SETTINGS, 0777);
    return;
}

extern "C" {
s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    pid = 0;
    struct proc_info procInfo;
    if (sys_sdk_proc_info(&procInfo) == 0) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        memcpy(game_elf, procInfo.name, sizeof(game_elf));
        memcpy(game_ver, procInfo.version, sizeof(game_ver));
        make_folders();
        print_proc_info();
        get_key_init();
        return 0;
    }
    NotifyStatic(TEX_ICON_SYSTEM, "Unable to get process info from " STRINGIFY(sys_sdk_proc_info));
    return -1;
}
}

extern "C" {
s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    return 0;
}
}
