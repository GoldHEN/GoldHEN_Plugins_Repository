#include "Common.h"

Detour* Detour_sceKernelAioInitializeImpl = nullptr;
Detour* Detour_sceKernelAioDeleteRequest = nullptr;
Detour* Detour_sceKernelAioDeleteRequests = nullptr;
Detour* Detour_sceKernelAioPollRequest = nullptr;
Detour* Detour_sceKernelAioPollRequests = nullptr;
Detour* Detour_sceKernelAioCancelRequest = nullptr;
Detour* Detour_sceKernelAioCancelRequests = nullptr;
Detour* Detour_sceKernelAioWaitRequest = nullptr;
Detour* Detour_sceKernelAioWaitRequests = nullptr;
Detour* Detour_sceKernelAioSubmitReadCommands = nullptr;
Detour* Detour_sceKernelAioSubmitReadCommandsMultiple = nullptr;
Detour* Detour_sceKernelAioSubmitWriteCommands = nullptr;
Detour* Detour_sceKernelAioSubmitWriteCommandsMultiple = nullptr;

#define MAX_QUEUE 512
#define SCE_KERNEL_AIO_STATE_SUBMITTED (1)
#define SCE_KERNEL_AIO_STATE_PROCESSING (2)
#define SCE_KERNEL_AIO_STATE_COMPLETED (3)
#define SCE_KERNEL_AIO_STATE_ABORTED (4)

typedef struct SceKernelAioResult {
    int64_t returnValue;
    uint32_t state;
} SceKernelAioResult;

typedef int SceKernelAioSubmitId;

typedef struct SceKernelAioRWRequest {
    off_t offset;
    size_t nbyte;
    void* buf;
    struct SceKernelAioResult* result;
    int fd;
} SceKernelAioRWRequest;

static int* id_state;
static int id_index;
// static pthread_mutex_t lock;

int (*sceKernelAioInitializeImpl)(void* p, int size);
int (*sceKernelAioDeleteRequest)(SceKernelAioSubmitId id, int* ret);
int (*sceKernelAioDeleteRequests)(SceKernelAioSubmitId id[], int num,
                                  int ret[]);

int (*sceKernelAioPollRequest)(SceKernelAioSubmitId id, int* state);
int (*sceKernelAioPollRequests)(SceKernelAioSubmitId id[], int num,
                                int state[]);

int (*sceKernelAioCancelRequest)(SceKernelAioSubmitId id, int* state);

int (*sceKernelAioCancelRequests)(SceKernelAioSubmitId id[], int num,
                                  int state[]);

int (*sceKernelAioWaitRequest)(SceKernelAioSubmitId id, int* state,
                               unsigned int* usec);
int (*sceKernelAioWaitRequests)(SceKernelAioSubmitId id[], int num, int state[],
                                uint32_t mode, unsigned int* usec);

int (*sceKernelAioSubmitReadCommands)(SceKernelAioRWRequest req[], int size,
                                      int prio, SceKernelAioSubmitId* id);

int (*sceKernelAioSubmitReadCommandsMultiple)(SceKernelAioRWRequest req[],
                                              int size, int prio,
                                              SceKernelAioSubmitId id[]);

int (*sceKernelAioSubmitWriteCommands)(SceKernelAioRWRequest req[], int size,
                                       int prio, SceKernelAioSubmitId* id);

int (*sceKernelAioSubmitWriteCommandsMultiple)(SceKernelAioRWRequest req[],
                                               int size, int prio,
                                               SceKernelAioSubmitId id[]);

/////hook code////

int sceKernelAioInitializeImpl_hook(void* p, int size) {
    id_index = 1;

    id_state = (int*)malloc(sizeof(int) * MAX_QUEUE);

    memset(id_state, 0, sizeof(sizeof(int) * MAX_QUEUE));

    return 0;
}

int sceKernelAioDeleteRequest_hook(SceKernelAioSubmitId id, int* ret) {
    id_state[id] = SCE_KERNEL_AIO_STATE_ABORTED;
    *ret = 0;
    return 0;
}

int sceKernelAioDeleteRequests_hook(SceKernelAioSubmitId id[], int num,
                                    int ret[]) {
    for (int i = 0; i < num; i++) {
        id_state[id[i]] = SCE_KERNEL_AIO_STATE_ABORTED;
        ret[i] = 0;
    }

    return 0;
}
int sceKernelAioPollRequest_hook(SceKernelAioSubmitId id, int* state) {
    *state = id_state[id];
    return 0;
}

int sceKernelAioPollRequests_hook(SceKernelAioSubmitId id[], int num,
                                  int state[]) {
    for (int i = 0; i < num; i++) {
        state[i] = id_state[id[i]];
    }

    return 0;
}

int sceKernelAioCancelRequest_hook(SceKernelAioSubmitId id, int* state) {
    if (id) {
        id_state[id] = SCE_KERNEL_AIO_STATE_ABORTED;
        *state = SCE_KERNEL_AIO_STATE_ABORTED;
    } else {
        *state = SCE_KERNEL_AIO_STATE_PROCESSING;
    }
    return 0;
}

int sceKernelAioCancelRequests_hook(SceKernelAioSubmitId id[], int num,
                                    int state[]) {
    for (int i = 0; i < num; i++) {
        if (id[i]) {
            id_state[id[i]] = SCE_KERNEL_AIO_STATE_ABORTED;
            state[i] = SCE_KERNEL_AIO_STATE_ABORTED;
        } else {
            state[i] = SCE_KERNEL_AIO_STATE_PROCESSING;
        }
    }

    return 0;
}

int sceKernelAioWaitRequest_hook(SceKernelAioSubmitId id, int* state,
                                 unsigned int* usec) {
    unsigned int timer = 0;

    int timeout = 0;

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

int sceKernelAioWaitRequests_hook(SceKernelAioSubmitId id[], int num,
                                  int state[], uint32_t mode,
                                  unsigned int* usec) {
    unsigned int timer = 0;
    int timeout = 0;
    int completion = 0;

    for (int i = 0; i < num; i++) {
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
            if (id_state[id[i]] == SCE_KERNEL_AIO_STATE_COMPLETED)
                completion = 1;
        }

        state[i] = id_state[id[i]];
    }

    if (timeout) return 0x8002003c;

    return 0;
}

int sceKernelAioSubmitReadCommands_hook(SceKernelAioRWRequest req[], int size,
                                        int prio, SceKernelAioSubmitId* id) {
    for (int i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;

        ssize_t ret =
            sceKernelPread(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

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

    if (!id_index) id_index++;

    return 0;
}

int sceKernelAioSubmitReadCommandsMultiple_hook(SceKernelAioRWRequest req[],
                                                int size, int prio,
                                                SceKernelAioSubmitId id[]) {
    for (int i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;

        ssize_t ret =
            sceKernelPread(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

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

int sceKernelAioSubmitWriteCommands_hook(SceKernelAioRWRequest req[], int size,
                                         int prio, SceKernelAioSubmitId* id) {
    for (int i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;

        ssize_t ret =
            sceKernelPwrite(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

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

int sceKernelAioSubmitWriteCommandsMultiple_hook(SceKernelAioRWRequest req[],
                                                 int size, int prio,
                                                 SceKernelAioSubmitId id[]) {
    for (int i = 0; i < size; i++) {
        id_state[id_index] = SCE_KERNEL_AIO_STATE_PROCESSING;
        ssize_t ret =
            sceKernelPwrite(req[i].fd, req[i].buf, req[i].nbyte, req[i].offset);

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

extern "C" {
int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(size_t argc, const void* args) {
    printf("[GoldHEN] <aio> module_start\n");

    int h = 0;

    if (sys_dynlib_load_prx("libkernel.sprx", &h))
        if (sys_dynlib_load_prx("libkernel_sys.sprx", &h))
            sys_dynlib_load_prx("libkernel_web.sprx", &h);

    sys_dynlib_dlsym(h, "sceKernelAioInitializeImpl",
                     &sceKernelAioInitializeImpl);
    sys_dynlib_dlsym(h, "sceKernelAioDeleteRequest",
                     &sceKernelAioDeleteRequest);
    sys_dynlib_dlsym(h, "sceKernelAioDeleteRequests",
                     &sceKernelAioDeleteRequests);
    sys_dynlib_dlsym(h, "sceKernelAioPollRequest", &sceKernelAioPollRequest);
    sys_dynlib_dlsym(h, "sceKernelAioPollRequests", &sceKernelAioPollRequests);
    sys_dynlib_dlsym(h, "sceKernelAioCancelRequest",
                     &sceKernelAioCancelRequest);
    sys_dynlib_dlsym(h, "sceKernelAioCancelRequests",
                     &sceKernelAioCancelRequests);
    sys_dynlib_dlsym(h, "sceKernelAioWaitRequest", &sceKernelAioWaitRequest);
    sys_dynlib_dlsym(h, "sceKernelAioWaitRequests", &sceKernelAioWaitRequests);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitReadCommands",
                     &sceKernelAioSubmitReadCommands);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitReadCommandsMultiple",
                     &sceKernelAioSubmitReadCommandsMultiple);
    sys_dynlib_dlsym(h, "sceKernelAioSubmitWriteCommands",
                     &sceKernelAioSubmitWriteCommands);
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

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(size_t argc, const void* args) {
    klog("[GoldHEN] <aio> module_stop\n");

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
}
