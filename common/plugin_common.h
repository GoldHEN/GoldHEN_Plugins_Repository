#include "git_ver.h"

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define s8  int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#ifdef FINAL
#define BUILD_TYPE "(Release)"
#define debug_printf(a, args...)
#else
#define BUILD_TYPE "(Debug)"
#define debug_printf(a, args...) printf("[%s] (%s:%d) " a,  __func__,__FILE__, __LINE__, ##args)
#endif
#define final_printf(a, args...) printf("(%s:%d) " a, __FILE__, __LINE__, ##args)
#define boot_ver() {\
  final_printf("Commit: %s Branch: %s Build: %i %s\n", GIT_COMMIT, GIT_VER, GIT_NUM, BUILD_TYPE);\
  final_printf("Built: %s\n", BUILD_DATE);\
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
