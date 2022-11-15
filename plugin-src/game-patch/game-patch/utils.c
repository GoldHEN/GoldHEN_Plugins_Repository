#include "utils.h"

s32 Read_File(const char *File, char **Data, size_t *Size, int extra) {
    s32 res = 0;
    s32 pos = 0;
    s32 fd = 0;

    debug_printf("Reading File \"%s\"\n", File);

    fd = sceKernelOpen(File, 0, 0777);
    if (fd < 0) {
        debug_printf("sceKernelOpen() 0x%08x\n", fd);
        res = fd;
        goto term;
    }

    *Size = sceKernelLseek(fd, 0, SEEK_END);
    if (*Size == 0) {
        debug_printf("ERROR: File is empty %i\n", res);
        res = -1;
        goto term;
    }

    res = sceKernelLseek(fd, 0, SEEK_SET);
    if (res < 0) {
        debug_printf("sceKernelLseek() 0x%08x\n", res);
        goto term;
    }

    *Data = (char *)malloc(*Size + extra);
    if (*Data == NULL) {
        debug_printf("ERROR: malloc()\n");
        goto term;
    }

    res = sceKernelRead(fd, *Data, *Size);
    if (res < 0) {
        debug_printf("sceKernelRead() 0x%08x\n", res);
        goto term;
    }

    res = sceKernelClose(fd);

    if (res < 0) {
        debug_printf("ERROR: sceKernelClose() 0x%08x\n", res);
        goto term;
    }

    debug_printf("File %s has been read - Res: %d - Size: %jd\n", File, res,
                 *Size);

    return res;

term:

    if (fd != 0) {
        sceKernelClose(fd);
    }

    return res;
}

s32 Write_File(const char *File, unsigned char *Data, size_t Size) {
    s32 fd = sceKernelOpen(File, 0x200 | 0x002, 0777);
    if (fd < 0) {
        debug_printf("Failed to make file \"%s\"\n", File);
        return 0;
    }
    debug_printf("Writing File \"%s\" %li\n", File, Size);
    s64 written = sceKernelWrite(fd, Data, Size);
    debug_printf("Written File \"%s\" %li\n", File, written);
    sceKernelClose(fd);
    return 1;
}
