#include <Common.h>
#include "../../../common/plugin_common.h"
#include "tiny-json.h"

enum patch_types
{
    kbyte = 0,
    kbytes16,
    kbytes32,
    kbytes64,
    kbytes,
    kfloat32,
    kfloat64,
    kutf8,
    kutf16,
    type_max
};

static const char* patch_type_str[] =
{
    [patch_types::kbyte]  = "byte",
    [patch_types::kbytes16]  = "bytes16",
    [patch_types::kbytes32]  = "bytes32",
    [patch_types::kbytes64]  = "bytes64",
    [patch_types::kbytes]  = "bytes",
    [patch_types::kfloat32]  = "float32",
    [patch_types::kfloat64]  = "float64",
    [patch_types::kutf8]  = "utf8",
    [patch_types::kutf16]  = "utf16",
};

const u32 max_tokens = 4096;

typedef struct {
    json_t mem[max_tokens];
    unsigned int nextFree;
    jsonPool_t pool;
} jsonStaticPool_t;

static json_t *poolInit(jsonPool_t *pool) {
    jsonStaticPool_t *spool = json_containerOf(pool, jsonStaticPool_t, pool);
    spool->nextFree = 1;
    return &spool->mem[0];
}

static json_t *poolAlloc(jsonPool_t *pool) {
    jsonStaticPool_t *spool = json_containerOf(pool, jsonStaticPool_t, pool);
    if (spool->nextFree >= sizeof spool->mem / sizeof spool->mem[0])
        return 0;
    return &spool->mem[spool->nextFree++];
}

unsigned char *hexstrtochar2(const char *hexstr, s64 &size);
int sys_proc_rw(uint64_t pid, uint64_t address, void *data, uint64_t length,
                uint64_t write = 1);
bool prefix(const char *pre, const char *str);
u64 hash(const char *str);
u64 patch_hash_calc(const char *title, const char *name, const char *app_ver,
                    const char *title_id, const char *elf = "");
