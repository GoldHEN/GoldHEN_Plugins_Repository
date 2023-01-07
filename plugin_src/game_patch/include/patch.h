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
u64 djb2(const char *str);
u64 patch_hash_calc(const char *title, const char *name, const char *app_ver,
                    const char *title_id, const char *elf);
void patch_data1(const char *type, u64 addr, const char *value);
