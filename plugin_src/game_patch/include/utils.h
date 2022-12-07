#include <Common.h>
#include "plugin_common.h"

s32 Read_File(const char *input_file, char **file_data, u64 *filesize, u32 extra);
s32 Write_File(const char *input_file, unsigned char *file_data, u64 filesize);
