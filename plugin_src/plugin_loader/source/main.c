// plugin-loader: Plugin loader for GoldHEN.
// Authors:
// Ctn123 @ https://github.com/Ctn123
// illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "GoldHEN.h"

#include <orbis/libkernel.h>

#include "plugin_common.h"
#include "config.h"

#define PLUGIN_CONFIG_PATH GOLDHEN_PATH "/plugins.ini"
#define PLUGIN_PATH GOLDHEN_PATH "/plugins"
#define PLUGIN_DEFAULT_SECTION "default"
#define PLUGIN_SETTINGS_SECTION "settings"

attr_public const char *g_pluginName = "plugin_loader";
attr_public const char *g_pluginDesc = "Plugin loader for GoldHEN";
attr_public const char *g_pluginAuth = "Ctn123, illusion";
attr_public u32 g_pluginVersion = 0x00000110; // 1.10

char g_PluginDetails[256] = {0};

// Todo: Move to sdk.
bool file_exists(const char* filename)
{
    struct stat buff;
    return stat(filename, &buff) == 0 ? true : false;
}

void create_template_config(void)
{
    final_printf("Creating new %s file\n", PLUGIN_CONFIG_PATH);
    // compile time
    #define DEFAULT_INI_DATA "" \
                            "[" PLUGIN_SETTINGS_SECTION "]\n" \
                            "; Global settings for plugin loader.\n" \
                            "; Affects every app/process boot.\n" \
                            "show_load_notification=true\n" \
                            "; Details for show_load_notification\n" \
                            "; Shows how many plugins were successfully loaded.\n" \
                            "; Valid options: false or true.\n" \
                            "\n" \
                            "; Load plugins in default section regardless of Title ID\n" \
                            "[default]\n" \
                            ";/data/GoldHEN/plugins/example.prx\n" \
                            ";/data/GoldHEN/plugins/example2.prx\n" \
                            "\n" \
                            "; Note: text following the ; are comments\n"

    // Does not work, may not have write access.
    //FILE* f = fopen(PLUGIN_CONFIG_PATH, "w");
    //fprintf(f, "%s", file_str);
    //fclose(f);

    // Works
    // Todo: Move to sdk.
    int32_t f = sceKernelOpen(PLUGIN_CONFIG_PATH, 0x200 | 0x001, 0777);
    if (f < 0)
    {
        final_printf("Failed to create file \"%s\" 0x%08x\n", PLUGIN_CONFIG_PATH, f);
        return;
    }

    sceKernelWrite(f, DEFAULT_INI_DATA, strlen(DEFAULT_INI_DATA));
    sceKernelClose(f);
}

bool simple_get_bool(const char* val)
{
    if (val == NULL || val[0] == 0)
    {
        return true;
    }
    if (!startsWith(val, "on") ||
        !startsWith(val, "true") ||
        !startsWith(val, "1"))
    {
        return true;
    }
    else
    {
        return false;
    }
    return true;
}

size_t strncat_s(char* dest, size_t destSize, const char* source, size_t source_count)
{
    if (dest == NULL || destSize == 0 || source == NULL || source_count == 0)
    {
        return 0;
    }

    size_t destLen = strlen(dest);
    size_t srcLen = strlen(source);

    if ((destLen + srcLen) >= destSize)
    {
        return 0;
    }

    size_t copyLen = destSize - destLen - 1;
    if (copyLen > source_count)
    {
        copyLen = source_count;
    }

    for (size_t i = 0; i < copyLen; i++)
    {
        dest[destLen + i] = source[i];
    }

    dest[destLen + copyLen] = '\0';
    return destLen + copyLen;
}

void load_plugins(ini_section_s *section, uint32_t *load_count)
{
    bool notifi_shown = false;
    for (uint32_t j = 0; j < section->size; j++)
    {
        ini_entry_s *entry = &section->entry[j];
        final_printf("%s=%s\n", entry->key, entry->value);
        if (!simple_get_bool(entry->value))
        {
            final_printf("Skipping entry (%s)\n", entry->value);
            continue;
        }
        if (entry->key[0] != '/')
        {
            char notify_msg[160] = {0};
            snprintf(notify_msg, sizeof(notify_msg), "Path:\n\"%s\"\nis wrong!\nPlugin will not load.", entry->key);
            if (!notifi_shown)
            {
                NotifyStatic(TEX_ICON_SYSTEM, notify_msg);
                notifi_shown = true;
            }
            else
            {
                final_printf("%s\n", notify_msg);
            }
            continue;
        }
        chmod(entry->key, 0777);
        sceKernelChmod(entry->key, 0777);
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
            int32_t ret = 0;
            bool load_success = false;
            const char** ModuleName = NULL;
            // TODO: accept user provided arguments
            int32_t(*plugin_load_ret)(void) = NULL;
            int32_t(*plugin_unload_ret)(void) = NULL;
            ret = sceKernelDlsym(result, "g_pluginName", (void**)&ModuleName);
            final_printf("Loaded Plugin %s\n", entry->key);
            final_printf("Plugin Handle 0x%08x Dlsym 0x%08x\n", result, ret);
            ret = sceKernelDlsym(result, "plugin_load", (void**)&plugin_load_ret);
            final_printf("plugin_load Dlsym 0x%08x @ 0x%p\n", ret, plugin_load_ret);
            ret = sceKernelDlsym(result, "plugin_unload", (void**)&plugin_unload_ret);
            final_printf("plugin_unload Dlsym 0x%08x @ 0x%p\n", ret, plugin_unload_ret);
            if (plugin_load_ret && plugin_unload_ret)
            {
                final_printf("Starting plugin...\n");
                int32_t prx_ret = plugin_load_ret();
                final_printf("plugin_load returned with 0x%08x\n", prx_ret);
                if (prx_ret || prx_ret < 0)
                {
                    final_printf("Program returned non zero, Starting plugin_unload...\n");
                    int32_t prx_ret = plugin_unload_ret();
                    final_printf("plugin_unload returned with 0x%08x\n", prx_ret);
                }
                else if (prx_ret == 0)
                {
                    final_printf("plugin_load exit successful 0x%08x\n", prx_ret);
                    *load_count += 1;
                    load_success = true;
                }
            }
            else
            {
                final_printf("Unable to find plugin_load or plugin_unload!\n");
                load_success = false;
                continue;
            }
            char plugin_entry[128] = {0};
            if (ModuleName && load_success)
            {
                snprintf(plugin_entry, sizeof(plugin_entry), "%u. %s\n", *load_count, *ModuleName);
            }
            else if (!ModuleName)
            {
                final_printf("Failed to resolve g_pluginName string!\n");
                snprintf(plugin_entry, sizeof(plugin_entry), "%u. unknown\n", *load_count);
            }
            else if (strlen(g_PluginDetails) >= sizeof(g_PluginDetails))
            {
                final_printf("String size of g_PluginDetails is too large!\n");
                final_printf("g_PluginDetails: %s\n", g_PluginDetails);
                final_printf("strlen(g_PluginDetails): %li\n", strlen(g_PluginDetails));
            }
            if (!strncat_s(g_PluginDetails, sizeof(g_PluginDetails), plugin_entry, strlen(plugin_entry)))
            {
                final_printf("Failed to concatenate string!\n");
                final_printf("g_PluginDetails: %s\n", g_PluginDetails);
                final_printf("strlen(g_PluginDetails): %li\n", strlen(g_PluginDetails));
                final_printf("sizeof(g_PluginDetails): %li\n", sizeof(g_PluginDetails));
                final_printf("plugin_entry: %s\n", plugin_entry);
                final_printf("strlen(plugin_entry): %li\n", strlen(plugin_entry));
            }
        }
    }
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

    // final_printf("Section is TitleID [%s]\n", procInfo.titleid);
    bool show_load_notification = false;
    uint32_t load_count = 0;

    for (uint32_t i = 0; i < config->size; i++)
    {
        ini_section_s *section = &config->section[i];

        if (section == NULL)
            continue;

        if (strcmp(section->name, PLUGIN_SETTINGS_SECTION) == 0)
        {
            final_printf("Section [%s] is settings\n", section->name);
            for (uint16_t j = 0; j < section->size; j++)
            {
                ini_entry_s *entry = &section->entry[j];
                final_printf("%s=%s\n", entry->key, entry->value);
                if (strcmp("show_load_notification", entry->key) == 0)
                {
                    show_load_notification = simple_get_bool(entry->value);
                    final_printf("%s=%u\n", entry->key, show_load_notification);
                }
            }
        }

        if (strcmp(section->name, PLUGIN_DEFAULT_SECTION) == 0)
        {
            final_printf("Section [%s] is default\n", section->name);
            load_plugins(section, &load_count);
        }
        else if (strcmp(section->name, procInfo.titleid) == 0)
        {
            final_printf("Section is TitleID [%s]\n", procInfo.titleid);
            load_plugins(section, &load_count);
        }
    }

    if (show_load_notification)
    {
        if (load_count > 0)
        {
            char notify_msg[128] = {0};
            // trim newline
            size_t PluginLen = strlen(g_PluginDetails) - 1;
            if (g_PluginDetails[PluginLen] == '\n')
            {
                g_PluginDetails[PluginLen] = '\0';
            }
            snprintf(notify_msg, sizeof(notify_msg), "Loaded %u plugin(s)\n%s", load_count, g_PluginDetails);
            NotifyStatic(TEX_ICON_SYSTEM, notify_msg);
        }
    }

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
