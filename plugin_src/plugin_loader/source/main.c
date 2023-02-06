// plugin-loader: Plugin loader for GoldHEN.
// Authors:
// Ctn123 @ https://github.com/Ctn123
// illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "GoldHEN.h"

#include <orbis/libkernel.h>

#include "plugin_common.h"
#include "config.h"

#define PLUGIN_CONFIG_PATH GOLDHEN_PATH "/plugins.ini"
#define PLUGIN_PATH GOLDHEN_PATH "/plugins"
#define PLUGIN_DEFAULT_SECTION "default"

attr_public const char *g_pluginName = "plugin_loader";
attr_public const char *g_pluginDesc = "Plugin loader for GoldHEN";
attr_public const char *g_pluginAuth = "Ctn123, illusion";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

// Todo: Move to sdk.
bool file_exists(const char* filename)
{
    struct stat buff;
    return stat(filename, &buff) == 0 ? true : false;
}

void create_template_config()
{
    final_printf("Creating new %s file\n", PLUGIN_CONFIG_PATH);
    const char *file_str = "; Load plugins under specific Title ID CUSA12345\n"
                           "[CUSA12345]\n"
                           ";/data/GoldHEN/plugins/example34.prx\n"
                           ";/data/GoldHEN/plugins/example138.prx\n"
                           "\n"
                           "; Load plugins in default section regardless of Title ID\n"
                           "[default]\n"
                           ";/data/GoldHEN/plugins/example.prx\n"
                           ";/data/GoldHEN/plugins/example2.prx\n"
                           "\n"
                           "; Note: text following the ; are comments\n";

    // Does not work, may not have write access.
    //FILE* f = fopen(PLUGIN_CONFIG_PATH, "w");
    //fprintf(f, "%s", file_str);
    //fclose(f);

    // Works
    // Todo: Move to sdk.
    int32_t f = sceKernelOpen(PLUGIN_CONFIG_PATH, 0x200 | 0x001, 0777);
    if (f < 0)
    {
        final_printf("Failed to create file \"%s\"\n", PLUGIN_CONFIG_PATH);
        return;
    }

    ssize_t written = sceKernelWrite(f, file_str, strlen(file_str));
    sceKernelClose(f);
}

uint16_t load_plugins(ini_section_s *section)
{
    uint16_t load_count = 0;
    for (uint16_t j = 0; j < section->size; j++)
    {
        ini_entry_s *entry = &section->entry[j];
        SceKernelChmod(entry->key, 0777);
        final_printf("Starting %s\n", entry->key);
        int32_t result = sceKernelLoadStartModule(entry->key, 0, 0, 0, NULL, NULL);
        if (result == 0x80020002)
        {
            final_printf("Plugin %s not found\n", entry->key);
        } else if (result < 0)
        {
            final_printf("Error loading Plugin %s! Error code 0x%08x (%i)\n", entry->key, result, result);
        } else
        {
            final_printf("Loaded Plugin %s\n", entry->key);
            load_count++;
        }
    }
    return load_count;
}

int32_t attr_module_hidden module_start(size_t argc, const void *args)
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();

    // Better done in GoldHEN
    mkdir(PLUGIN_PATH, 0777);

    struct proc_info procInfo;

    if (sys_sdk_proc_info(&procInfo) == 0)
    {
        print_proc_info();
    } else
    {
        final_printf("Plugin Manager failed to initialise\n");
        return -1;
    }

    final_printf("Plugin Manager started successfully\n");


    // Better done in GoldHEN
    if (!file_exists(PLUGIN_CONFIG_PATH))
    {
       final_printf("Plugin config %s not found\n", PLUGIN_CONFIG_PATH);
       create_template_config();
       return -1;
    }


    ini_table_s *config = ini_table_create();
    if (config == NULL)
    {
        final_printf("Config parser failed to initialise\n");
        return -1;
    }

    if (!ini_table_read_from_file(config, PLUGIN_CONFIG_PATH))
    {
        final_printf("Config parser failed to parse config: %s\n", PLUGIN_CONFIG_PATH);
        return -1;
    }

    /*
    ini_section_s *section = _ini_section_find(config, PLUGIN_DEFAULT_SECTION);
    if (section == NULL)
    {
        final_printf("%s section not found\n", PLUGIN_DEFAULT_SECTION);
        return -1;
    }
    */

    final_printf("Section is TitleID [%s]\n", procInfo.titleid);

    uint16_t load_count = 0;

    for (uint16_t i = 0; i < config->size; i++)
    {
        ini_section_s *section = &config->section[i];

        if (section == NULL)
            continue;

        if (strcmp(section->name, PLUGIN_DEFAULT_SECTION) == 0)
        {
            final_printf("Section [%s] is default\n", section->name);
            load_count += load_plugins(section);
        } else if (strcmp(section->name, procInfo.titleid) == 0)
        {
            final_printf("Section is TitleID [%s]\n", procInfo.titleid);
            load_count += load_plugins(section);
        }
    }
    final_printf("Loaded %u plugin(s)\n", load_count);

    if (config != NULL)
        ini_table_destroy(config);

    return 0;
}

int32_t attr_module_hidden module_stop(size_t argc, const void *args)
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("Plugin Manager ended successfully\n");
    return 0;
}
