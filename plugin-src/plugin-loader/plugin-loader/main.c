#include "common.h"
#include "config.h"
#include "../../../common/plugin_common.h"

#define PLUGIN_PATH "/data/GoldHEN/plugins"
#define SETTINGS_PATH PLUGIN_PATH "/settings"
#define "/data/GoldHEN/plugins.ini"
#define DEFAULT_INI_SECTION "default"
#define plugin_name "boot-loader"

char titleid[16] = {0};

void load_plugins(ini_section_s *section) {
    for (int j = 0; j < section->size; j++) {
        ini_entry_s *entry = &section->entry[j];
        final_printf("Starting %s\n", entry->key);
        int result = sceKernelLoadStartModule(entry->key, 0, 0, 0, NULL, NULL);
        if (result == 0x80020002) {
            final_printf("Plugin %s not found\n", entry->key);
        } else if (result < 0) {
            final_printf("Error loading Plugin %s! with error code %i\n", entry->key, result);
        } else {
            final_printf("Loaded Plugin %s.\n", entry->key);
        }
    }
    return;
}

int __attribute__((weak)) __attribute__((visibility("hidden"))) module_start(size_t argc, const void *args) {
    final_printf("[GoldHEN] <%s> %s\n", plugin_name, __func__);
    boot_ver();
    mkdir(SETTINGS_PATH, 0777);
    struct proc_info procInfo;
    if (sys_sdk_proc_info(&procInfo) == 0) {
        memcpy(titleid, procInfo.titleid, sizeof(titleid));
        print_proc_info();
    } else {
        final_printf("Plugin Manager failed to initialise\n");
        return -1;
    }

    final_printf("Plugin Manager started successfully\n");
    ini_table_s *config = ini_table_create();
    if (config == NULL) {
        final_printf("Config parser failed to initialise\n");
        return -1;
    }

    if (!ini_table_read_from_file(config, CONFIG_PATH)) {
        final_printf("Config parser failed to parse config: %s\n", CONFIG_PATH);
        return -1;
    }

    ini_section_s *section = _ini_section_find(config, DEFAULT_INI_SECTION);
    if (section == NULL) {
        final_printf("%s section not found\n", DEFAULT_INI_SECTION);
        return -1;
    }
    // int load_count = 0;
    // Example looping over sections and keys
    for (int i = 0; i < config->size; i++) {
        ini_section_s *section = &config->section[i];
        if (section == NULL)
            continue;
        if (strcmp(section->name, DEFAULT_INI_SECTION) == 0) {
            final_printf("Section [%s] is default\n", section->name);
            load_plugins(section);
        }
        if (strcmp(section->name, titleid) == 0) {
            final_printf("Section is TitleID [%s]\n", titleid);
            load_plugins(section);
        }
    }

    // final_printf("Loaded %i plugin(s)\n", load_count);
    // always reported zero for some reason, even when passing in as refernce
    // removed for now
    if (config != NULL) {
        ini_table_destroy(config);
    }
    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden"))) module_stop(size_t argc, const void *args) {
    final_printf("Plugin Manager ended successfully\n");
    return 0;
}
