#include <Common.h>
#include "../../../common/plugin_common.h"
#include "tiny-json.h"

void Notify(const char *FMT, ...);
s32 Read_File(const char *File, char **Data, size_t *Size, int extra);
s32 Write_File(const char *File, unsigned char *Data, size_t Size);