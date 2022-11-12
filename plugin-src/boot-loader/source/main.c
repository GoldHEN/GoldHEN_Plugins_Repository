#include "common.h"
#include "config.h"
#include "../../../common/plugin_common.h"
#define PLUGIN_PATH "/data/GoldHEN/plugins"
#define CONFIG_PATH PLUGIN_PATH "/goldhen.ini"
#define GOLDHEN_INI_SECTION "GoldHEN_plugins"
#define plugin_name "boot-loader"

int __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    boot_ver();
    final_printf("Plugin Manager started successfully!\n");

    ini_table_s *config = ini_table_create();
    if (config == NULL) {
        final_printf("Config parser failed to initialise!\n");
        return -1;
    }

    if (!ini_table_read_from_file(config, CONFIG_PATH)) {
        final_printf("Config parser failed to parse config!\n");
        return -1;
    }

    ini_section_s *section = _ini_section_find(config, GOLDHEN_INI_SECTION);
    if (section == NULL) {
        final_printf("%s section not found!\n", GOLDHEN_INI_SECTION);
        return -1;
    }

    int load_count = 0;
    for (int i = 0; i < section->size; i++) {
        // this expects crlf
        ini_entry_s *entry = &section->entry[i];
        final_printf("Starting %s\n", entry->key);
        int result = sceKernelLoadStartModule(entry->key, 0, 0, 0, NULL, NULL);
        if (result == 0x80020002) {
            final_printf(" -> Plugin %s not found!\n", entry->key);
        } else if (result < 0) {
            final_printf(" -> Error loading Plugin %s! with error code %i\n", entry->key, result);
        } else {
            final_printf(" -> Loaded Plugin %s.\n", entry->key);
            load_count++;
        }
    }

    final_printf("Loaded %i plugin(s)\n", load_count);

    // Example looping over sections and keys
    /*
    for (int i = 0; i < config->size; i++)
    {
        ini_section_s* section = &config->section[i];
        if (section == NULL) continue;

        final_printf("Section [%s]\n", section->name);
        if (strcmp(section->name, "GoldHEN_plugins") != 0)
            continue;

        for (int j = 0; j < section->size; j++)
        {
            ini_entry_s* entry = &section->entry[j];
            if (access(plugin_path, F_OK) != 0)
                final_printf("Plugin %s not found!\n", entry->key);
                continue;
            final_printf("Starting %s\n", entry->key);
            sceKernelLoadStartModule(entry->key, 0, 0, 0, NULL, NULL);
        }
    }
    */

    if (config != NULL) {
        ini_table_destroy(config);
    }

    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
    final_printf("Plugin Manager ended successfully!\n");

    return 0;
}
