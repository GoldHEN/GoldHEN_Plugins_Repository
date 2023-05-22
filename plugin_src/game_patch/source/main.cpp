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

uint64_t module_base = 0;
uint32_t module_size = 0;
// unused for now
uint64_t PRX_module_base = 0;
uint32_t PRX_module_size = 0;

const char* GetXMLAttr(mxml_node_t *node, const char *name)
{
    const char* AttrData = mxmlElementGetAttr(node, name);
    if (AttrData == NULL) AttrData = "";
    return AttrData;
}

constexpr uint32_t MAX_PATTERN_LENGTH = 256;

static int pattern_to_byte(const char* pattern, uint8_t* bytes)
{
    int count = 0;
    const char* start = pattern;
    const char* end = pattern + strlen(pattern);

    for (const char* current = start; current < end; ++current)
    {
        if (*current == '?')
        {
            ++current;
            if (*current == '?')
            {
                ++current;
            }
            bytes[count++] = -1;
        }
        else
        {
            bytes[count++] = strtoul(current, (char**)&current, 16);
        }
    }
    return count;
}

/*
 * @brief Scan for a given byte pattern on a module
 *
 * @param module    Base of the module to search
 * @param signature IDA-style byte array pattern
 *
 * @returns Address of the first occurrence
 */
 // https://github.com/OneshotGH/CSGOSimple-master/blob/59c1f2ec655b2fcd20a45881f66bbbc9cd0e562e/CSGOSimple/helpers/utils.cpp#L182
uint8_t* PatternScan(uint64_t module_base, uint32_t module_size, const char* signature)
{
    if (!module_base || !module_size)
        return nullptr;
    uint8_t patternBytes[MAX_PATTERN_LENGTH];
    int patternLength = pattern_to_byte(signature, patternBytes);
    uint8_t* scanBytes = (uint8_t*)module_base;

    for (size_t i = 0; i < module_size; ++i)
    {
        bool found = true;
        for (int j = 0; j < patternLength; ++j)
        {
            if (scanBytes[i + j] != patternBytes[j] && patternBytes[j] != 0xff)
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return &scanBytes[i];
        }
    }
    return NULL;
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
        mxml_node_t *node, *tree = NULL;
        tree = mxmlLoadString(NULL, buffer, MXML_NO_CALLBACK);

        if (!tree) {
            final_printf("XML: could not parse XML:\n%s\n", buffer);
            free(buffer);
            return;
        }

        for (node = mxmlFindElement(tree, tree, "Metadata", NULL, NULL, MXML_DESCEND); node != NULL;
             node = mxmlFindElement(node, tree, "Metadata", NULL, NULL, MXML_DESCEND)) {
            u8 use_mask = false;
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
            } else if (buffer2[0] == '1' && !strcmp(game_elf, AppElfData)) {
                s32 ret_cmp = strcmp(game_ver, AppVerData);
                if (!ret_cmp)
                {
                    use_mask = false;
                }
                else if (!strcmp("mask", AppVerData))
                {
                    use_mask = true;
                }
                else if (ret_cmp)
                {
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
                    const char *gameType = GetXMLAttr(Line_node, "Type");
                    const char *gameAddr = GetXMLAttr(Line_node, "Address");
                    const char *gameValue = GetXMLAttr(Line_node, "Value");
                    const char *gameOffset = nullptr;
                    if (use_mask)
                    {
                        if (!strcmp("mask_jump32", gameType))
                        {
                            const char* gameJumpTarget = GetXMLAttr(Line_node, "Target");
                            const char* gameJumpSize = GetXMLAttr(Line_node, "Size");
                            jump_addr = addr_real = (uint64_t)PatternScan(module_base, module_size, gameJumpTarget);
                            jump_size = strtoul(gameJumpSize, NULL, 10);
                            debug_printf("Target: 0x%lx jump size %u\n", jump_addr, jump_size);
                        }
                        gameOffset = GetXMLAttr(Line_node, "Offset");
                        addr_real = (uint64_t)PatternScan(module_base, module_size, gameAddr);
                        debug_printf("Masked Address: 0x%lx\n", addr_real);
                        debug_printf("Offset: %s\n", gameOffset);
                        if (gameOffset[0] != '0')
                        {
                            if (gameOffset[0] == '-')
                            {
                                debug_printf("Offset mode: subtract\n");
                            }
                            else if (gameOffset[0] == '+')
                            {
                                debug_printf("Offset mode: addition\n");
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
                    debug_printf("patch line: %lu\n", patch_lines);
                    if (gameType && addr_real) // type and address must be present
                    {
                        patch_data1(djb2_hash(gameType), addr_real, gameValue, gameOffset, jump_size, jump_addr);
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

// https://github.com/bucanero/apollo-ps4/blob/a530cae3c81639eedebac606c67322acd6fa8965/source/orbis_jbc.c#L62
int get_module_info(OrbisKernelModuleInfo moduleInfo, const char* name, uint64_t *base, uint32_t *size)
{
    OrbisKernelModule handles[256];
    size_t numModules;
    int ret = 0;
    ret = sceKernelGetModuleList(handles, sizeof(handles), &numModules);
    if (ret)
    {
        final_printf("sceKernelGetModuleList (0x%08x)\n", ret);
        return ret;
    }
    final_printf("numModules: %li\n", numModules);
    for (size_t i = 0; i < numModules; ++i)
    {
        ret = sceKernelGetModuleInfo(handles[i], &moduleInfo);
        final_printf("ret 0x%x\n", ret);
        final_printf("module %li\n", i);
        final_printf("name: %s\n", moduleInfo.name);
        final_printf("start: 0x%lx\n", (uint64_t)moduleInfo.segmentInfo[0].address);
        final_printf("size: %u (0x%x)\n", moduleInfo.segmentInfo[0].size, moduleInfo.segmentInfo[0].size);
        if (ret)
        {
            final_printf("sceKernelGetModuleInfo (%X)\n", ret);
            return ret;
        }

        if (strcmp(moduleInfo.name, name) == 0 || name[0] == '0')
        {
            if (base)
                *base = (uint64_t)moduleInfo.segmentInfo[0].address;

            if (size)
                *size = moduleInfo.segmentInfo[0].size;
            return 1;
        }
    }
    return 0;
}

extern "C" {
s32 attr_module_hidden module_start(s64 argc, const void *args) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();
    pid = 0;
    struct proc_info procInfo;
    OrbisKernelModuleInfo CurrentModuleInfo;
    CurrentModuleInfo.size = sizeof(OrbisKernelModuleInfo);
    if(!get_module_info(CurrentModuleInfo, "0", &module_base, &module_size) && module_base && module_size)
    {
        final_printf("Could not find module info for current process\n");
        return -1;
    }
    final_printf("Module start: 0x%lx 0x%x\n", module_base, module_size);
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
