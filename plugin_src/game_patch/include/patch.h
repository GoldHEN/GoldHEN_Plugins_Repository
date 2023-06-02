#include <Common.h>
#include "plugin_common.h"
#include <stdbool.h>

#pragma once

unsigned char *hexstrtochar2(const char *hexstr, s64 *size);
void sys_proc_rw(u64 address, void *data, u64 length);
bool hex_prefix(const char *str);

// http://www.cse.yorku.ca/~oz/hash.html
constexpr inline u64 djb2_hash(const char *str);

u64 patch_hash_calc(const char *title, const char *name, const char *app_ver,
                    const char *title_id, const char *elf);
void patch_data1(const char* patch_type_str, u64 addr, const char *value, uint32_t source_size, uint64_t jump_target);
