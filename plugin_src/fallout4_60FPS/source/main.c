#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Common.h>
#include "plugin_common.h"
#include "patch.h"

#define bool int
#define true 1
#define false 0
#define nullptr NULL

  /////////////////////////////////////////////////////////////
 //                   Plugin Metadata                       //          
/////////////////////////////////////////////////////////////

attr_public const char *g_pluginName = "Fallout 4 60FPS";
attr_public const char *g_pluginDesc = "Fixes UI positioning for Fallout 4 resolution patch";
attr_public const char *g_pluginAuth = "Boaz";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

  /////////////////////////////////////////////////////////////
 //                  Patch Definitions                      //          
/////////////////////////////////////////////////////////////

// Resolution Dimensions patch - directly writes width/height values
static const uint64_t RES_DIMS_OFFSETS[] = {
    0x022dd59c - 0x400000,  // width_addr offset
    0x022dd5a0 - 0x400000   // height_addr offset
};

// Resolution dimension values
static const uint32_t RES_WIDTH = 1920;
static const uint32_t RES_HEIGHT = 1080;

// Combined width/height data for direct write
static uint32_t g_res_dims_data[2];

// Resolved addresses for resolution dimensions patch
static uint64_t g_res_dims_addresses[2];

// Render Scaling patch data - scales render resolution to 0.625x
static const uint32_t RES_X_SCALE = 0x3f200000; // Horizontal: 1 -> 0.625 (DEFAULT: 0x3f800000)
static const uint32_t RES_Y_SCALE = 0x3f200000; // Vertical: 1 -> 0.625 (DEFAULT: 0x3f800000)

// Build render scaling patch data
static uint8_t g_res_patch_data[8];

// Ramp patch - replaces VADDSS with VMOVSS to stop upscaler increment
static const uint8_t RAMP_PATCH_DATA[16] = {
    0xc5, 0xfa, 0x10, 0x8f, 0xdc, 0x0f, 0x00, 0x00,  // VMOVSS XMM1, [RDI+0xfdc]
    0xc5, 0xfa, 0x10, 0x87, 0xd8, 0x0f, 0x00, 0x00   // VMOVSS XMM0, [RDI+0xfd8]
};

// FPS unlock patch - changes 0x00 to 0x01 at fliprate to unlock 60 FPS cap
static const uint8_t FPS_PATCH_DATA[1] = { 0x00 };

// Patch definitions
static Patch g_patches[] = {
    {
        .name = "Resolution Dimensions",
        .pattern = NULL,
        .data = (uint8_t*)g_res_dims_data,
        .data_size = sizeof(g_res_dims_data),
        .direct_addresses = g_res_dims_addresses,
        .address_count = sizeof(g_res_dims_addresses) / sizeof(g_res_dims_addresses[0])
    },
    {
        .name = "Render Scaling",
        .pattern = "21 31 c0 eb 1f 48 b8 00 00 80 3f 00 00 80 3f 48 89 87 d8 0f 00 00 c6 87 00 10 00 00 01 e9 96 01 00 00",
        .data = g_res_patch_data,
        .data_size = 8,
        .offset = 7,
        .max_occurrences = 1
    },
    {
        .name = "Ramp Control",
        .pattern = "c5 fa 58 8f dc 0f 00 00 c5 fa 58 87 d8 0f 00 00 c5 fa 10 15",
        .data = RAMP_PATCH_DATA,
        .data_size = sizeof(RAMP_PATCH_DATA),
        .offset = 0,
        .max_occurrences = 1
    },
    {
        .name = "60 FPS Unlock",
        .pattern = "c7 05 0f 9f 4f 02 01 00 00 00",
        .data = FPS_PATCH_DATA,
        .data_size = 1,
        .offset = 6,
        .max_occurrences = 1
    }
};

static const uint32_t G_PATCH_COUNT = sizeof(g_patches) / sizeof(g_patches[0]);

  /////////////////////////////////////////////////////////////
 //                   Module Functions                      //          
/////////////////////////////////////////////////////////////

s32 get_module_info(const char* name, uint64_t *base, uint32_t *size) {
    OrbisKernelModule handles[256];
    size_t numModules;
    s32 ret = 0;
    OrbisKernelModuleInfo moduleInfo = {0};

    ret = sceKernelGetModuleList(handles, sizeof(handles), &numModules);
    if (ret) {
        final_printf("sceKernelGetModuleList (0x%08x)\n", ret);
        return ret;
    }
    final_printf("numModules: %li\n", numModules);
    for (size_t i = 0; i < numModules; ++i) {
        moduleInfo.size = sizeof(moduleInfo);
        ret = sceKernelGetModuleInfo(handles[i], &moduleInfo);
        if (ret) {
            final_printf("sceKernelGetModuleInfo (0x%08x)\n", ret);
            return ret;
        }
        final_printf("module name: %s start: 0x%lx size: 0x%x\n", moduleInfo.name, (uint64_t)moduleInfo.segmentInfo[0].address, moduleInfo.segmentInfo[0].size);
        if (strstr(moduleInfo.name, name) != NULL || strstr(moduleInfo.name, "eboot") != NULL) {
            if (base)
                *base = (uint64_t)moduleInfo.segmentInfo[0].address;
            if (size)
                *size = moduleInfo.segmentInfo[0].size;
            return 1;
        }
    }
    return 0;
}

s32 get_process_base(uint64_t *base, uint32_t *size) {
    struct proc_info procInfo = {0};
    if (sys_sdk_proc_info(&procInfo) != 0) {
        final_printf("sys_sdk_proc_info failed\n");
        return 0;
    }
    final_printf("process name: %s titleid: %s base_address: 0x%lx\n", procInfo.name, procInfo.titleid, procInfo.base_address);
    if (base)
        *base = procInfo.base_address;
    if (size)
        *size = 0;
    return 1;
}

int32_t attr_public plugin_load(int32_t argc, const char* argv[]) {
    final_printf("[GoldHEN] %s Plugin Started.\n", g_pluginName);
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);

    Notify(TEX_ICON_SYSTEM, "%s loaded", g_pluginName);

    uint64_t module_base = 0;
    uint32_t module_size = 0;
    if (get_module_info("eboot.bin", &module_base, &module_size) != 1) {
        Notify(TEX_ICON_SYSTEM, "Failed to get eboot.bin module info\n");
        return -1;
    }
    if (!module_size) {
        module_size = 0x2200000;
    }

    // Initialize all patch data
    memcpy(g_res_dims_data, &RES_WIDTH, 4);
    memcpy((uint8_t*)g_res_dims_data + 4, &RES_HEIGHT, 4);
    
    // Calculate actual addresses for resolution dimensions patch
    g_res_dims_addresses[0] = module_base + RES_DIMS_OFFSETS[0];
    g_res_dims_addresses[1] = module_base + RES_DIMS_OFFSETS[1];
    
    memcpy(g_res_patch_data, &RES_X_SCALE, 4);
    memcpy(g_res_patch_data + 4, &RES_Y_SCALE, 4);

    // Apply all configured patches
    for (uint32_t i = 0; i < G_PATCH_COUNT; ++i) {
        if (apply_patch(module_base, module_size, &g_patches[i]) < 0) {
            // Only return error for critical patches (render scaling)
            if (i == 1) {
                return -1;
            }
        }
    }

    Notify(TEX_ICON_SYSTEM, "[DONE] Fallout 4 - 60FPS by Boaz - Enjoy :)");
    return 0;
}

int32_t attr_public plugin_unload(int32_t argc, const char* argv[]) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] %s Plugin Ended.\n", g_pluginName);
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args) {
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args) {
    return 0;
}