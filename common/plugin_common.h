#include "git_ver.h"
#include <Common.h>

#define GOLDHEN_PATH "/data/GoldHEN"

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define s8  int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define f32 float
#define f64 double
#define TEX_ICON "cxml://psnotification/tex_icon_system"

#define attr_module_hidden __attribute__((weak)) __attribute__((visibility("hidden")))
#define attr_public __attribute__((visibility("default")))

#if (__FINAL__) == 1
#define BUILD_TYPE "(Release)"
#define debug_printf(a, args...)
#else
#define BUILD_TYPE "(Debug)"
#define debug_printf(a, args...) klog("[%s] (%s:%d) " a,  __func__,__FILE__, __LINE__, ##args)
#endif

#define final_printf(a, args...) klog("(%s:%d) " a, __FILE__, __LINE__, ##args)
#define boot_ver() {\
    final_printf("Commit: %s Branch: %s Build: %i %s\n", GIT_COMMIT, GIT_VER, GIT_NUM, BUILD_TYPE);\
    final_printf("Built: %s\n", BUILD_DATE);\
    final_printf("GoldHEN SDK Ver: 0x%08x\n", GOLDHEN_SDK_VERSION);\
    final_printf("GoldHEN System SDK Ver: 0x%08x\n", sys_sdk_version());\
}

#define print_proc_info() {\
    final_printf("process info\n");\
    final_printf("pid: %d\n", procInfo.pid);\
    final_printf("name: %s\n", procInfo.name);\
    final_printf("path: %s\n", procInfo.path);\
    final_printf("titleid: %s\n", procInfo.titleid);\
    final_printf("contentid: %s\n", procInfo.contentid);\
    final_printf("version: %s\n", procInfo.version);\
    final_printf("base_address: 0x%lx\n", procInfo.base_address);\
}

void Notify(const char* IconUri, const char *FMT, ...);
