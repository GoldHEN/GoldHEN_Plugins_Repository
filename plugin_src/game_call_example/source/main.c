// Game Call Example: Calling game function example.

// Author: icemesh @ https://github.com/icemesh
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "plugin_common.h"
#include <orbis/libkernel.h>

attr_public const char *g_pluginName = "game_call_example";
attr_public const char *g_pluginDesc = "Calling game function example";
attr_public const char *g_pluginAuth = "icemesh, illusion";
attr_public uint32_t g_pluginVersion = 0x00000100; // 1.00

struct proc_info procInfo;
#define NO_ASLR_ADDR 0x00400000

void* my_thread(void* args)
{
    uint64_t startPtr = procInfo.base_address;
    // Define game functions with parameters.
    void (*MyFunc1) (int param_1) = ((void(*) (int param_1)) (startPtr + (0x0224f980 - NO_ASLR_ADDR)));
    void (*MyFunc2) (const char *char_format, ...) = ((void(*)(const char *char_format, ...)) (startPtr + (0x0224f980 - NO_ASLR_ADDR)));
    uint32_t boot_wait = 10;
    final_printf("Sleeping for %u seconds...\n", boot_wait);
    sleep(boot_wait);
    while (true)
    {
        MyFunc1(2);
        MyFunc2("Hello from void* my_thread() (msgcon)\n");
        sceKernelUsleep(33333);
    }
    scePthreadExit(NULL);
    return NULL;
}

int32_t attr_module_hidden module_start(size_t argc, const void *args)
{
    if (sys_sdk_proc_info(&procInfo) == 0)
    {
        print_proc_info();
    }
    else
    {
        final_printf("Failed to get process info\n");
        return 0;
    }
    OrbisPthread thread;
    final_printf("[GoldHEN] %s Plugin Started.\n", g_pluginName);
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    scePthreadCreate(&thread, NULL, my_thread, NULL, "my_thread");
    return 0;
}

int32_t attr_module_hidden module_stop(size_t argc, const void *args)
{
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] %s Plugin Ended.\n", g_pluginName);
    return 0;
}
