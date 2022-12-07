#include "utils.h"

s32 Read_File(const char *input_file, char **file_data, u64 *filesize, u32 extra) {
    s32 res = 0;
    s32 pos = 0;
    s32 fd = 0;

    debug_printf("Reading input_file \"%s\"\n", input_file);

    fd = sceKernelOpen(input_file, 0, 0777);
    if (fd < 0) {
        debug_printf("sceKernelOpen() 0x%08x\n", fd);
        res = fd;
        goto term;
    }

    *filesize = sceKernelLseek(fd, 0, SEEK_END);
    if (*filesize == 0) {
        debug_printf("ERROR: input_file is empty %i\n", res);
        res = -1;
        goto term;
    }

    res = sceKernelLseek(fd, 0, SEEK_SET);
    if (res < 0) {
        debug_printf("sceKernelLseek() 0x%08x\n", res);
        goto term;
    }

    *file_data = (char *)malloc(*filesize + extra);
    if (*file_data == NULL) {
        debug_printf("ERROR: malloc()\n");
        goto term;
    }

    res = sceKernelRead(fd, *file_data, *filesize);
    if (res < 0) {
        debug_printf("sceKernelRead() 0x%08x\n", res);
        goto term;
    }

    res = sceKernelClose(fd);

    if (res < 0) {
        debug_printf("ERROR: sceKernelClose() 0x%08x\n", res);
        goto term;
    }

    debug_printf("input_file %s has been read - Res: %d - filesize: %jd\n", input_file, res,
                 *filesize);

    return res;

term:

    if (fd != 0) {
        sceKernelClose(fd);
    }

    return res;
}

s32 Write_File(const char *input_file, unsigned char *file_data, u64 filesize) {
    s32 fd = 0;
    s64 size_written = 0;
    fd = sceKernelOpen(input_file, 0x200 | 0x002, 0777);
    if (fd < 0) {
        debug_printf("Failed to make file \"%s\"\n", input_file);
        return 0;
    }
    debug_printf("Writing input_file \"%s\" %li\n", input_file, filesize);
    size_written = sceKernelWrite(fd, file_data, filesize);
    debug_printf("Written input_file \"%s\" %li\n", input_file, size_written);
    sceKernelClose(fd);
    return 1;
}
