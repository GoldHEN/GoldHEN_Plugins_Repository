#pragma once

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>

#include "Syscall.h"
#include "GoldHEN.h"
#include "Utilities.h"
#include "Detour.h"

#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
