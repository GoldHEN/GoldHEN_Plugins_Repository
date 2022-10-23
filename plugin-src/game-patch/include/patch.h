#include <Common.h>
#include "../../../common/plugin_common.h"
#include "tiny-json.h"

#define max_tokens 4096

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

extern int pid;
extern u8 arr8[1];
extern u8 arr16[2];
extern u8 arr32[4];
extern u8 arr64[8];

extern char titleid[16];
extern char game_elf[32];
extern char game_ver[8];

unsigned char *hexstrtochar2(const char *hexstr, s64 *size);
int sys_proc_rw(uint64_t pid, uint64_t address, void *data, uint64_t length);
bool prefix(const char *pre, const char *str);
u64 hash(const char *str);
u64 patch_hash_calc(const char *title, const char *name, const char *app_ver,
                    const char *title_id, const char *elf);
void patch_data1(const char *type, u64 addr, const char *value);
