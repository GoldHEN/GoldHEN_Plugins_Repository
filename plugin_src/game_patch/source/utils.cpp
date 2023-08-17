#include "utils.h"

s32 Read_File(const char *input_file, char **file_data, u64 *filesize, u32 extra) {
    s32 res = 0;
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

// https://github.com/bucanero/apollo-ps4/blob/a530cae3c81639eedebac606c67322acd6fa8965/source/orbis_jbc.c#L62
s32 get_module_info(OrbisKernelModuleInfo moduleInfo, const char* name, uint64_t *base, uint32_t *size)
{
    OrbisKernelModule handles[256];
    size_t numModules;
    s32 ret = 0;
    ret = sceKernelGetModuleList(handles, sizeof(handles), &numModules);
    if (ret)
    {
        final_printf("sceKernelGetModuleList (0x%08x)\n", ret);
        return ret;
    }
    final_printf("numModules: %li\n", numModules);
    for (size_t i = 0; i < numModules; ++i)
    {
        ret = sceKernelGetModuleInfo(handles[i], &moduleInfo);
        final_printf("ret 0x%x\n", ret);
        final_printf("module %li\n", i);
        final_printf("name: %s\n", moduleInfo.name);
        final_printf("start: 0x%lx\n", (uint64_t)moduleInfo.segmentInfo[0].address);
        final_printf("size: %u (0x%08x) bytes\n", moduleInfo.segmentInfo[0].size, moduleInfo.segmentInfo[0].size);
        if (ret)
        {
            final_printf("sceKernelGetModuleInfo (%X)\n", ret);
            return ret;
        }

        if (strcmp(moduleInfo.name, name) == 0 || name[0] == '0')
        {
            if (base)
                *base = (uint64_t)moduleInfo.segmentInfo[0].address;

            if (size)
                *size = moduleInfo.segmentInfo[0].size;
            return 1;
        }
    }
    return 0;
}

u32 pattern_to_byte(const char* pattern, uint8_t* bytes)
{
    u32 count = 0;
    const char* start = pattern;
    const char* end = pattern + strlen(pattern);

    for (const char* current = start; current < end; ++current)
    {
        if (*current == '?')
        {
            ++current;
            if (*current == '?')
            {
                ++current;
            }
            bytes[count++] = -1;
        }
        else
        {
            bytes[count++] = strtoul(current, (char**)&current, 16);
        }
    }
    return count;
}

/*
 * @brief Scan for a given byte pattern on a module
 *
 * @param module_base Base of the module to search
 * @param module_size Size of the module to search
 * @param signature   IDA-style byte array pattern
 * @credit            https://github.com/OneshotGH/CSGOSimple-master/blob/59c1f2ec655b2fcd20a45881f66bbbc9cd0e562e/CSGOSimple/helpers/utils.cpp#L182
 * @returns           Address of the first occurrence
 */
u8* PatternScan(uint64_t module_base, uint32_t module_size, const char* signature)
{
    if (!module_base || !module_size)
    {
        return nullptr;
    }
    constexpr u32 MAX_PATTERN_LENGTH = 256;
    u8 patternBytes[MAX_PATTERN_LENGTH] = { 0 };
    s32 patternLength = pattern_to_byte(signature, patternBytes);
    if (!patternLength || patternLength >= MAX_PATTERN_LENGTH)
    {
        final_printf("Pattern length too large or invalid! %i (0x%08x)\n", patternLength, patternLength);
        final_printf("Input Pattern %s\n", signature);
        return nullptr;
    }
    u8* scanBytes = (uint8_t*)module_base;

    for (u64 i = 0; i < module_size; ++i)
    {
        bool found = true;
        for (s32 j = 0; j < patternLength; ++j)
        {
            if (scanBytes[i + j] != patternBytes[j] && patternBytes[j] != 0xff)
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return &scanBytes[i];
        }
    }
    return nullptr;
}
