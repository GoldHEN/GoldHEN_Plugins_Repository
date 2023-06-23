// Author: jocover @ https://github.com/jocover
// Repository: https://github.com/GoldHEN/GoldHEN_Plugins_Repository

#include "plugin_common.h"
#include "Common.h"

attr_public const char *g_pluginName = "async_io_fix";
attr_public const char *g_pluginDesc = "(null)";
attr_public const char *g_pluginAuth = "jocover";
attr_public u32 g_pluginVersion = 0x00000100; // 1.00

HOOK_INIT(sceKernelAioInitializeImpl);
HOOK_INIT(sceKernelAioDeleteRequest);
HOOK_INIT(sceKernelAioDeleteRequests);
HOOK_INIT(sceKernelAioPollRequest);
HOOK_INIT(sceKernelAioPollRequests);
HOOK_INIT(sceKernelAioCancelRequest);
HOOK_INIT(sceKernelAioCancelRequests);
HOOK_INIT(sceKernelAioWaitRequest);
HOOK_INIT(sceKernelAioWaitRequests);
HOOK_INIT(sceKernelAioSubmitReadCommands);
HOOK_INIT(sceKernelAioSubmitReadCommandsMultiple);
HOOK_INIT(sceKernelAioSubmitWriteCommands);
HOOK_INIT(sceKernelAioSubmitWriteCommandsMultiple);

#define MAX_QUEUE 512
#define SCE_KERNEL_AIO_STATE_SUBMITTED (1)
#define SCE_KERNEL_AIO_STATE_PROCESSING (2)
#define SCE_KERNEL_AIO_STATE_COMPLETED (3)
#define SCE_KERNEL_AIO_STATE_ABORTED (4)

typedef struct SceKernelAioResult {
    s64 returnValue;
    u32 state;
} SceKernelAioResult;

typedef s32 SceKernelAioSubmitId;

typedef struct SceKernelAioRWRequest {
    off_t offset;
    s64 nbyte;
    void* buf;
    struct SceKernelAioResult* result;
    s32 fd;
} SceKernelAioRWRequest;

static s32* id_state;
static s32 id_index;
// static pthread_mutex_t lock;

s32 (*sceKernelAioInitializeImpl)(void* p, s32 size);
s32 (*sceKernelAioDeleteRequest)(SceKernelAioSubmitId id, s32* ret);
s32 (*sceKernelAioDeleteRequests)(SceKernelAioSubmitId id[], s32 num, s32 ret[]);

s32 (*sceKernelAioPollRequest)(SceKernelAioSubmitId id, s32* state);
s32 (*sceKernelAioPollRequests)(SceKernelAioSubmitId id[], s32 num, s32 state[]);

s32 (*sceKernelAioCancelRequest)(SceKernelAioSubmitId id, s32* state);

s32 (*sceKernelAioCancelRequests)(SceKernelAioSubmitId id[], s32 num, s32 state[]);

s32 (*sceKernelAioWaitRequest)(SceKernelAioSubmitId id, s32* state, u32* usec);
s32 (*sceKernelAioWaitRequests)(SceKernelAioSubmitId id[], s32 num, s32 state[], u32 mode,
                                u32* usec);

s32 (*sceKernelAioSubmitReadCommands)(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                      SceKernelAioSubmitId* id);

s32 (*sceKernelAioSubmitReadCommandsMultiple)(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                              SceKernelAioSubmitId id[]);

s32 (*sceKernelAioSubmitWriteCommands)(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                       SceKernelAioSubmitId* id);

s32 (*sceKernelAioSubmitWriteCommandsMultiple)(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                               SceKernelAioSubmitId id[]);

/////hook code////

s32 sceKernelAioInitializeImpl_hook(void* p, s32 size) {

    return 0;
}

s32 sceKernelAioDeleteRequest_hook(SceKernelAioSubmitId id, s32* ret) {
    id_state[id] = SCE_KERNEL_AIO_STATE_ABORTED;
    *ret = 0;
    return 0;
}

s32 sceKernelAioDeleteRequests_hook(SceKernelAioSubmitId id[], s32 num, s32 ret[]) {
    for (s32 i = 0; i < num; i++) {
        id_state[id[i]] = SCE_KERNEL_AIO_STATE_ABORTED;
        ret[i] = 0;
    }

    return 0;
}
s32 sceKernelAioPollRequest_hook(SceKernelAioSubmitId id, s32* state) {
    *state = id_state[id];
    return 0;
}

s32 sceKernelAioPollRequests_hook(SceKernelAioSubmitId id[], s32 num, s32 state[]) {
    for (s32 i = 0; i < num; i++) {
        state[i] = id_state[id[i]];
    }

    return 0;
}

s32 sceKernelAioCancelRequest_hook(SceKernelAioSubmitId id, s32* state) {
    if (id) {
        id_state[id] = SCE_KERNEL_AIO_STATE_ABORTED;
        *state = SCE_KERNEL_AIO_STATE_ABORTED;
    } else {
        *state = SCE_KERNEL_AIO_STATE_PROCESSING;
    }
    return 0;
}

s32 sceKernelAioCancelRequests_hook(SceKernelAioSubmitId id[], s32 num, s32 state[]) {
    for (s32 i = 0; i < num; i++) {
        if (id[i]) {
            id_state[id[i]] = SCE_KERNEL_AIO_STATE_ABORTED;
            state[i] = SCE_KERNEL_AIO_STATE_ABORTED;
        } else {
            state[i] = SCE_KERNEL_AIO_STATE_PROCESSING;
        }
    }

    return 0;
}

s32 sceKernelAioWaitRequest_hook(SceKernelAioSubmitId id, s32* state, u32* usec) {
    u32 timer = 0;

    s32 timeout = 0;

    while (id_state[id] == SCE_KERNEL_AIO_STATE_PROCESSING) {
        sceKernelUsleep(10);

        timer += 10;
        if (*usec) {
            if (timer > *usec) {
                timeout = 1;
                break;
            }
        }
    }

    *state = id_state[id];

    if (timeout) return 0x8002003c;
    return 0;
}

s32 sceKernelAioWaitRequests_hook(SceKernelAioSubmitId id[], s32 num, s32 state[], u32 mode,
                                  u32* usec) {
    u32 timer = 0;
    s32 timeout = 0;
    s32 completion = 0;

    for (s32 i = 0; i < num; i++) {
        if (!completion && !timeout) {
            while (id_state[id[i]] == SCE_KERNEL_AIO_STATE_PROCESSING) {
                sceKernelUsleep(10);
                timer += 10;

                if (*usec) {
                    if (timer > *usec) {
                        timeout = 1;
                        break;
                    }
                }
            }
        }

        if (mode == 0x02) {
            if (id_state[id[i]] == SCE_KERNEL_AIO_STATE_COMPLETED) completion = 1;
        }

        state[i] = id_state[id[i]];
    }

    if (timeout) return 0x8002003c;

    return 0;
}

s32 sceKernelAioSubmitReadCommands_hook(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                        SceKernelAioSubmitId* id) {

    id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;

    for (s32 i = 0; i < size; i++) {

        s64 ret = sceKernelPread(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

        if (ret < 0) {
            req[i].result->state = SCE_KERNEL_AIO_STATE_ABORTED;
            req[i].result->returnValue = ret;

        } else {
            req[i].result->state = SCE_KERNEL_AIO_STATE_COMPLETED;
            req[i].result->returnValue = ret;

        }
    }

    id_state[id_index] = SCE_KERNEL_AIO_STATE_COMPLETED;

    *id = id_index;

    id_index = (id_index + 1) % MAX_QUEUE;

    if (!id_index) id_index++;

    return 0;
}

s32 sceKernelAioSubmitReadCommandsMultiple_hook(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                                SceKernelAioSubmitId id[]) {
    for (s32 i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;

        s64 ret = sceKernelPread(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

        if (ret < 0) {
            req[i].result->state = SCE_KERNEL_AIO_STATE_ABORTED;
            req[i].result->returnValue = ret;

            id_state[id_index] = SCE_KERNEL_AIO_STATE_ABORTED;

        } else {
            req[i].result->state = SCE_KERNEL_AIO_STATE_COMPLETED;
            req[i].result->returnValue = ret;

            id_state[id_index] = SCE_KERNEL_AIO_STATE_COMPLETED;
        }

        id[i] = id_index;

        id_index = (id_index + 1) % MAX_QUEUE;

        if (!id_index) id_index++;
    }

    return 0;
}

s32 sceKernelAioSubmitWriteCommands_hook(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                         SceKernelAioSubmitId* id) {
    for (s32 i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;

        s64 ret = sceKernelPwrite(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

        if (ret < 0) {
            req[i].result->state = SCE_KERNEL_AIO_STATE_ABORTED;
            req[i].result->returnValue = ret;

            id_state[id_index] = SCE_KERNEL_AIO_STATE_ABORTED;

        } else {
            req[i].result->state = SCE_KERNEL_AIO_STATE_COMPLETED;
            req[i].result->returnValue = ret;

            id_state[id_index] = SCE_KERNEL_AIO_STATE_COMPLETED;
        }
    }

    *id = id_index;

    id_index = (id_index + 1) % MAX_QUEUE;

    // skip id_index equals 0 , because sceKernelAioCancelRequest will submit id
    // equal to 0
    if (!id_index) id_index++;

    return 0;
}

s32 sceKernelAioSubmitWriteCommandsMultiple_hook(SceKernelAioRWRequest req[], s32 size, s32 prio,
                                                 SceKernelAioSubmitId id[]) {
    for (s32 i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;
        s64 ret = sceKernelPwrite(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

        if (ret < 0) {
            req[i].result->state = SCE_KERNEL_AIO_STATE_ABORTED;
            req[i].result->returnValue = ret;

            id_state[id_index] = SCE_KERNEL_AIO_STATE_ABORTED;

        } else {
            req[i].result->state = SCE_KERNEL_AIO_STATE_COMPLETED;
            req[i].result->returnValue = ret;
            id_state[id_index] = SCE_KERNEL_AIO_STATE_COMPLETED;
        }

        id[i] = id_index;
        id_index = (id_index + 1) % MAX_QUEUE;

        if (!id_index) id_index++;
    }
    return 0;
}

s32 attr_public plugin_load(s32 argc, const char* argv[]) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
    boot_ver();

    id_index = 1;
    id_state = (int*)malloc(sizeof(int) * MAX_QUEUE);
    memset(id_state, 0, sizeof(sizeof(int) * MAX_QUEUE));
    int h = 0;

    if (sys_dynlib_load_prx("libkernel.sprx", &h))
        if (sys_dynlib_load_prx("libkernel_sys.sprx", &h))
            sys_dynlib_load_prx("libkernel_web.sprx", &h);

    sys_dynlib_dlsym(h, "sceKernelAioInitializeImpl", &sceKernelAioInitializeImpl);
    sys_dynlib_dlsym(h, "sceKernelAioDeleteRequest", &sceKernelAioDeleteRequest);
    sys_dynlib_dlsym(h, "sceKernelAioDeleteRequests", &sceKernelAioDeleteRequests);
    sys_dynlib_dlsym(h, "sceKernelAioPollRequest", &sceKernelAioPollRequest);
    sys_dynlib_dlsym(h, "sceKernelAioPollRequests", &sceKernelAioPollRequests);
    sys_dynlib_dlsym(h, "sceKernelAioCancelRequest", &sceKernelAioCancelRequest);
    sys_dynlib_dlsym(h, "sceKernelAioCancelRequests", &sceKernelAioCancelRequests);
    sys_dynlib_dlsym(h, "sceKernelAioWaitRequest", &sceKernelAioWaitRequest);
    sys_dynlib_dlsym(h, "sceKernelAioWaitRequests", &sceKernelAioWaitRequests);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitReadCommands", &sceKernelAioSubmitReadCommands);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitReadCommandsMultiple",
                     &sceKernelAioSubmitReadCommandsMultiple);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitWriteCommands", &sceKernelAioSubmitWriteCommands);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitWriteCommandsMultiple",
                     &sceKernelAioSubmitWriteCommandsMultiple);

    HOOK(sceKernelAioInitializeImpl);
    HOOK(sceKernelAioDeleteRequest);
    HOOK(sceKernelAioDeleteRequests);
    HOOK(sceKernelAioPollRequest);
    HOOK(sceKernelAioPollRequests);
    HOOK(sceKernelAioCancelRequest);
    HOOK(sceKernelAioCancelRequests);
    HOOK(sceKernelAioWaitRequest);
    HOOK(sceKernelAioWaitRequests);
    HOOK(sceKernelAioSubmitReadCommands);
    HOOK(sceKernelAioSubmitReadCommandsMultiple);
    HOOK(sceKernelAioSubmitWriteCommands);
    HOOK(sceKernelAioSubmitWriteCommandsMultiple);
    return 0;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[]) {
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    UNHOOK(sceKernelAioInitializeImpl);
    UNHOOK(sceKernelAioDeleteRequest);
    UNHOOK(sceKernelAioDeleteRequests);
    UNHOOK(sceKernelAioPollRequest);
    UNHOOK(sceKernelAioPollRequests);
    UNHOOK(sceKernelAioCancelRequest);
    UNHOOK(sceKernelAioCancelRequests);
    UNHOOK(sceKernelAioWaitRequest);
    UNHOOK(sceKernelAioWaitRequests);
    UNHOOK(sceKernelAioSubmitReadCommands);
    UNHOOK(sceKernelAioSubmitReadCommandsMultiple);
    UNHOOK(sceKernelAioSubmitWriteCommands);
    UNHOOK(sceKernelAioSubmitWriteCommandsMultiple);
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    return 0;
}
