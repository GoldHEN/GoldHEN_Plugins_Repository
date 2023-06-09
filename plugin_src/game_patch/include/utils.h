#include <Common.h>
#include "plugin_common.h"

s32 Read_File(const char *input_file, char **file_data, u64 *filesize, u32 extra);
s32 Write_File(const char *input_file, unsigned char *file_data, u64 filesize);

s32 get_module_info(OrbisKernelModuleInfo moduleInfo, const char* name, uint64_t *base, uint32_t *size);
u32 pattern_to_byte(const char* pattern, uint8_t* bytes);

/*
 * @brief Scan for a given byte pattern on a module
 *
 * @param module_base Base of the module to search
 * @param module_size Size of the module to search
 * @param signature   IDA-style byte array pattern
 * @credit            https://github.com/OneshotGH/CSGOSimple-master/blob/59c1f2ec655b2fcd20a45881f66bbbc9cd0e562e/CSGOSimple/helpers/utils.cpp#L182
 * @returns           Address of the first occurrence
 */
u8* PatternScan(uint64_t module_base, uint32_t module_size, const char* signature);
