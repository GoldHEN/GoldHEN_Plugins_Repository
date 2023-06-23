// Game Patch: Patches game before boot.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <mxml.h>
#include "patch.h"
#include "utils.h"

#define GOLDHEN_PATH_ (const char*) GOLDHEN_PATH
#define BASE_PATH_PATCH (const char*) GOLDHEN_PATH_ "/patches"
#define BASE_PATH_PATCH_SETTINGS (const char*) BASE_PATH_PATCH "/settings"
#define BASE_PATH_PATCH_XML (const char*) BASE_PATH_PATCH "/xml"
#define PLUGIN_NAME (const char*) "game_patch"
#define PLUGIN_DESC (const char*) "Patches game at boot"
#define PLUGIN_AUTH (const char*) "illusion"
#define PLUGIN_VER 0x110 // 1.10

#define NO_ASLR_ADDR 0x00400000

attr_public const char *g_pluginName = PLUGIN_NAME;
attr_public const char *g_pluginDesc = PLUGIN_DESC;
attr_public const char *g_pluginAuth = PLUGIN_AUTH;
attr_public u32 g_pluginVersion = PLUGIN_VER;

char g_titleid[16] = {0};
char g_game_elf[MAX_PATH_] = {0};
char g_game_prx[MAX_PATH_] = {0};
char g_game_ver[8] = {0};

u64 g_module_base = 0;
u32 g_module_size = 0;
// unused for now
bool g_PRX = false;
u64 g_PRX_module_base = 0;
u32 g_PRX_module_size = 0;

const char* GetXMLAttr(mxml_node_t *node, const char *name)
{
    const char* AttrData = mxmlElementGetAttr(node, name);
    if (AttrData == NULL) AttrData = "\0";
    return AttrData;
}

void get_key_init(void)
{
    u32 patch_lines = 0;
    u32 patch_items = 0;
    char *buffer = nullptr;
    char *buffer2 = nullptr;
    u64 size = 0;
    u64 size2 = 0;
    char input_file[MAX_PATH_] = {0};
    snprintf(input_file, sizeof(input_file), BASE_PATH_PATCH_XML "/%s.xml", g_titleid);
    s32 res = Read_File(input_file, &buffer, &size, 0);

    if (res)
    {
        final_printf("file %s not found\n", input_file);
        final_printf("error: 0x%08x\n", res);
        return;
    }

    if (buffer)
    {
        mxml_node_t *node, *tree = NULL;
        tree = mxmlLoadString(NULL, buffer, MXML_NO_CALLBACK);

        if (!tree)
        {
            final_printf("XML: could not parse XML:\n%s\n", buffer);
            free(buffer);
            return;
        }

        for (node = mxmlFindElement(tree, tree, "Metadata", NULL, NULL, MXML_DESCEND); node != NULL;
             node = mxmlFindElement(node, tree, "Metadata", NULL, NULL, MXML_DESCEND))
        {
            bool PRX_patch = false;
            const char *TitleData = GetXMLAttr(node, "Title");
            const char *NameData = GetXMLAttr(node, "Name");
            const char *AppVerData = GetXMLAttr(node, "AppVer");
            const char *AppElfData = GetXMLAttr(node, "AppElf");

            debug_printf("Title: \"%s\"\n", TitleData);
            debug_printf("Name: \"%s\"\n", NameData);
            debug_printf("AppVer: \"%s\"\n", AppVerData);
            debug_printf("AppElf: \"%s\"\n", AppElfData);

            u64 hashout = patch_hash_calc(TitleData, NameData, AppVerData, input_file, AppElfData);
            char settings_path[MAX_PATH_] = {0};
            snprintf(settings_path, sizeof(settings_path), BASE_PATH_PATCH_SETTINGS "/0x%016lx.txt", hashout);
            final_printf("Settings path: %s\n", settings_path);
            s32 res = Read_File(settings_path, &buffer2, &size2, 0);
            if (res == ORBIS_KERNEL_ERROR_ENOENT) {
                debug_printf("file %s not found, initializing false. ret: 0x%08x\n", settings_path, res);
                u8 false_data[] = {'0', '\n'};
                Write_File(settings_path, false_data, sizeof(false_data));
            } else if (buffer2[0] == '1' && !strcmp(g_game_elf, AppElfData)) {
                s32 ret_cmp = strcmp(g_game_ver, AppVerData);
                if (!ret_cmp)
                {
                    debug_printf("App ver %s == %s\n", g_game_ver, AppVerData);
                }
                else if (!startsWith(AppVerData, "mask") || !startsWith(AppVerData, "all"))
                {
                    debug_printf("App ver masked: %s\n", AppVerData);
                }
                else if (ret_cmp)
                {
                    debug_printf("App ver %s != %s\n", g_game_ver, AppVerData);
                    debug_printf("Skipping patch entry\n");
                    continue;
                }
                patch_items++;
                mxml_node_t *Patchlist_node = mxmlFindElement(node, node, "PatchList", NULL, NULL, MXML_DESCEND);
                for (mxml_node_t *Line_node = mxmlFindElement(node, node, "Line", NULL, NULL, MXML_DESCEND); Line_node != NULL;
                                  Line_node = mxmlFindElement(Line_node, Patchlist_node, "Line", NULL, NULL, MXML_DESCEND))
                {
                    u64 addr_real = 0;
                    u64 jump_addr = 0;
                    u32 jump_size = 0;
                    bool use_mask = false;
                    const char *gameType = GetXMLAttr(Line_node, "Type");
                    const char *gameAddr = GetXMLAttr(Line_node, "Address");
                    const char *gameValue = GetXMLAttr(Line_node, "Value");
                    const char *gameOffset = nullptr;
                    // starts with `mask`
                    if (!startsWith(gameType, "mask"))
                    {
                        use_mask = true;
                    }
                    if (use_mask)
                    {
                        if (!startsWith(gameType, "mask_jump32"))
                        {
                            const char* gameJumpTarget = GetXMLAttr(Line_node, "Target");
                            const char* gameJumpSize = GetXMLAttr(Line_node, "Size");
                            jump_addr = addr_real = (uint64_t)PatternScan(g_module_base, g_module_size, gameJumpTarget);
                            jump_size = strtoul(gameJumpSize, NULL, 10);
                            debug_printf("Target: 0x%lx jump size %u\n", jump_addr, jump_size);
                        }
                        gameOffset = GetXMLAttr(Line_node, "Offset");
                        addr_real = (uint64_t)PatternScan(g_module_base, g_module_size, gameAddr);
                        if (!addr_real)
                        {
                            final_printf("Masked Address: %s not found\n", gameAddr);
                            continue;
                        }
                        final_printf("Masked Address: 0x%lx\n", addr_real);
                        debug_printf("Offset: %s\n", gameOffset);
                        u32 real_offset = 0;
                        if (gameOffset[0] != '0')
                        {
                            if (gameOffset[0] == '-')
                            {
                                debug_printf("Offset mode: subtract\n");
                                real_offset = strtoul(gameOffset + 1, NULL, 10);
                                debug_printf("before offset: 0x%lx\n", addr_real);
                                addr_real = addr_real - real_offset;
                                debug_printf("after offset: 0x%lx\n", addr_real);
                            }
                            else if (gameOffset[0] == '+')
                            {
                                debug_printf("Offset mode: addition\n");
                                real_offset = strtoul(gameOffset + 1, NULL, 10);
                                debug_printf("before offset: 0x%lx\n", addr_real);
                                addr_real = addr_real + real_offset;
                                debug_printf("after offset: 0x%lx\n", addr_real);
                            }
                        }
                        else
                        {
                            debug_printf("Mask does not reqiure offsetting.\n");
                        }
                    }
                    debug_printf("Type: \"%s\"\n", gameType);
                    if (gameAddr && !use_mask)
                    {
                        addr_real = strtoull(gameAddr, NULL, 16);
                        debug_printf("Address: 0x%lx\n", addr_real);
                    }
                    debug_printf("Value: \"%s\"\n", gameValue);
                    debug_printf("patch line: %u\n", patch_lines);
                    if (gameType && addr_real && *gameValue != '\0') // type, address and value must be present
                    {
                        if (!PRX_patch && !use_mask)
                        {
                            // previous self, eboot patches were made with no aslr addresses
                            addr_real = g_module_base + (addr_real - NO_ASLR_ADDR);
                        }
                        else if (PRX_patch && !use_mask)
                        {
                            addr_real = g_module_base + addr_real;
                        }
                        patch_data1(gameType, addr_real, gameValue, jump_size, jump_addr);
                        patch_lines++;
                    }
                }
            }
        }

        mxmlDelete(node);
        mxmlDelete(tree);
        free(buffer);

        if (patch_items > 0 && patch_lines > 0)
        {
            char msg[128] = {0};
            snprintf(msg, sizeof(msg), "%u %s Applied\n"
                                       "%u %s Applied",
                                       patch_items, (patch_items == 1) ? "Patch" : "Patches",
                                       patch_lines, (patch_lines == 1) ? "Patch Line" : "Patch Lines");
            NotifyStatic(TEX_ICON_SYSTEM, msg);
        }
    }
    else // if (!buffer)
    {
        char msg[128] = {0};
        snprintf(msg, sizeof(msg), "File %s\nis empty", input_file);
        NotifyStatic(TEX_ICON_SYSTEM, msg);
    }
    if (buffer2)
    {
        free(buffer2);
    }
}

void mkdir_chmod(const char *path, OrbisKernelMode mode)
{
    sceKernelMkdir(path, mode);
    sceKernelChmod(path, mode);
}

void make_folders(void)
{
    mkdir_chmod(GOLDHEN_PATH_, 0777);
    mkdir_chmod(BASE_PATH_PATCH, 0777);
    mkdir_chmod(BASE_PATH_PATCH_XML, 0777);
    mkdir_chmod(BASE_PATH_PATCH_SETTINGS, 0777);
}

extern "C" {
s32 attr_public plugin_load(s32 argc, const char* argv[]) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    proc_info procInfo{};
    OrbisKernelModuleInfo CurrentModuleInfo{};
    CurrentModuleInfo.size = sizeof(OrbisKernelModuleInfo);
    if(!get_module_info(CurrentModuleInfo, "0", &g_module_base, &g_module_size) && (!g_module_base || !g_module_size))
    {
        final_printf("Could not find module info for current process\n");
        return -1;
    }
    final_printf("Module start: 0x%lx 0x%x\n", g_module_base, g_module_size);
    if (sys_sdk_proc_info(&procInfo) == 0)
    {
        strncpy(g_titleid, procInfo.titleid, sizeof(g_titleid));
        strncpy(g_game_elf, procInfo.name, sizeof(g_game_elf));
        strncpy(g_game_ver, procInfo.version, sizeof(g_game_ver));
        make_folders();
        print_proc_info();
        get_key_init();
        return 0;
    }
    NotifyStatic(TEX_ICON_SYSTEM, "Unable to get process info from sys_sdk_proc_info");
    return -1;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[]) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    return 0;
}
}
