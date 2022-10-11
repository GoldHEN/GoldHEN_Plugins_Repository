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
#define debug_printf(a, args...)
#else
#define debug_printf(a, args...) printf("[%s] (%s:%d) " a,  __func__,__FILE__, __LINE__, ##args)
#endif
#define final_printf(a, args...) printf("(%s:%d) " a, __FILE__, __LINE__, ##args)
#define boot_ver() final_printf("commit: %s version: %s build: %i\n", GIT_COMMIT, GIT_VER, GIT_NUM)
