#include <Common.h>
#include "plugin_common.h"
#include <stdbool.h>

extern s32 pid;

extern char titleid[16];
extern char game_elf[32];
extern char game_ver[8];

unsigned char *hexstrtochar2(const char *hexstr, s64 *size);
void sys_proc_rw(u64 address, void *data, u64 length);
bool hex_prefix(const char *str);

// http://www.cse.yorku.ca/~oz/hash.html
constexpr inline u64 djb2_hash(const char *str) {
    u64 hash = 5381;
    u32 c = 0;
    while ((c = *str++))
        hash = hash * 33 ^ c;
    return hash;
}

u64 patch_hash_calc(const char *title, const char *name, const char *app_ver,
                    const char *title_id, const char *elf);
void patch_data1(u64 patch_type, u64 addr, const char *value, const char *mask_offset, uint32_t source_size, uint64_t jump_target);
