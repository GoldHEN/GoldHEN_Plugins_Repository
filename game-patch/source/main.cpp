// Game Patch: Patches game before boot.
// Author: illusion0001 @ https://github.com/illusion0001
// Repository: https://github.com/illusion0001/ps4-plugins

#include "../include/patch.h"
#include "tiny-json.h"
#include <Common.h>
#include "../../common/plugin_common.h"

const u32 max_tokens = 4096;

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

const char *type_byte = "byte";
const char *type_bytes16 = "bytes16";
const char *type_bytes32 = "bytes32";
const char *type_bytes64 = "bytes64";
const char *type_bytes = "bytes";
const char *type_float32 = "float32";
const char *type_float64 = "float64";
const char *type_utf8 = "utf8";
const char *type_utf16 = "utf16";

const char *key_title = "title";
const char *key_app_ver = "app_ver";
const char *key_patch_ver = "patch_ver";
const char *key_name = "name";
const char *key_author = "author";
const char *key_note = "note";
const char *key_app_titleid = "app_titleid";
const char *key_app_elf = "app_elf";
const char *key_app_type = "type";
const char *key_app_addr = "addr";
const char *key_app_value = "value";
const char *key_patch_list = "patch_list";

const char *base_path = "/data/GoldHEN";
const char *base_path_patch = "/data/GoldHEN/patches";
const char *base_path_patch_json = "/data/GoldHEN/patches/json";
const char *base_path_patch_settings = "/data/GoldHEN/patches/settings";

json_t const *json_key_title;
json_t const *json_key_app_ver;
json_t const *json_key_patch_ver;
json_t const *json_key_name;
json_t const *json_key_author;
json_t const *json_key_note;
json_t const *json_key_app_titleid;
json_t const *json_key_app_elf;

char const *title_val;
char const *app_ver_val;
char const *patch_ver_val;
char const *name_val;
char const *author_val;
char const *note_val;
char const *app_titleid_val;
char const *app_elf_val;

// memory stuff
int pid = 0;
u8 arr8[1];
u8 arr16[2];
u8 arr32[4];
u8 arr64[8];
u64 patch_items = 0;

char titleid[16] = {0};
char game_elf[32] = {0};
char game_ver[8] = {0};

void Notify(const char *FMT, ...) {
    OrbisNotificationRequest Buffer;
    va_list args;
    va_start(args, FMT);
    vsprintf(Buffer.message, FMT, args);
    va_end(args);
    final_printf("Buffer.message: %s\n", Buffer.message);
    Buffer.type = OrbisNotificationRequestType::NotificationRequest;
    Buffer.unk3 = 0;
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    strcpy(Buffer.iconUri, "cxml://psnotification/tex_icon_system");
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}

unsigned char *hexstrtochar2(const char *hexstr, s64 &size) {
    // valid hex look up table.
    const uint8_t hex_lut[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00};

    uint32_t str_len = strlen(hexstr);
    size_t data_len = ((str_len + 1) / 2) * sizeof(unsigned char);

    size = (str_len) * sizeof(unsigned char);

    unsigned char *data = (unsigned char *)malloc(size);
    uint32_t j = 0; // hexstr position
    uint32_t i = 0; // data position

    if (str_len % 2 == 1) {
        data[i] = (uint8_t)(hex_lut[0] << 4) | hex_lut[(uint8_t)hexstr[j]];
        j = ++i;
    }

    for (; j < str_len; j += 2, i++) {
        data[i] = (uint8_t)(hex_lut[(uint8_t)hexstr[j]] << 4) |
                  hex_lut[(uint8_t)hexstr[j + 1]];
    }

    size = data_len;

    return data;
}

int sys_proc_rw(uint64_t pid, uint64_t address, void *data, uint64_t length,
                uint64_t write = 1) {
    // debug_printf("writing to memory at 0x%x\n", address);
    return orbis_syscall(108 + 90, pid, address, data, length, write);
}

const char *hex_prefix = "0x";
// https://stackoverflow.com/a/4770992
bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

void patch_data1(const char *type, u64 addr, const char *value) {
    if (type) {
        int str_base = 16;
        if (strcmp(type, patch_type_str[kbyte]) == 0) {
            u8 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtol(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtol(value, NULL, str_base);
            }
            memcpy(arr8, &real_value, sizeof(real_value));
            sys_proc_rw(pid, addr, arr8, 1);
            return;
        } else if (strcmp(type, patch_type_str[kbytes16]) == 0) {
            u16 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtol(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtol(value, NULL, str_base);
            }
            memcpy(arr16, &real_value, sizeof(real_value));
            sys_proc_rw(pid, addr, arr16, 2);
            return;
        } else if (strcmp(type, patch_type_str[kbytes32]) == 0) {
            u32 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtol(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtol(value, NULL, str_base);
            }
            memcpy(arr32, &real_value, sizeof(real_value));
            sys_proc_rw(pid, addr, arr32, 4);
            return;
        } else if (strcmp(type, patch_type_str[kbytes64]) == 0) {
            s64 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtoll(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtoll(value, NULL, str_base);
            }
            memcpy(arr64, &real_value, sizeof(real_value));
            sys_proc_rw(pid, addr, arr64, 8);
            return;
        } else if (strcmp(type, patch_type_str[kbytes]) == 0) {
            s64 szb = 0;
            u8 *bytearray = hexstrtochar2(value, szb);
            sys_proc_rw(pid, addr, bytearray, szb);
            return;
        } else if (strcmp(type, patch_type_str[kfloat32]) == 0) {
            debug_printf("type: %s unsupported", type);
            // strtod, atof crashes
            return;
        } else if (strcmp(type, patch_type_str[kfloat64]) == 0) {
            debug_printf("type: %s unsupported", type);
            // strtod, atof crashes
            return;
        } else if (strcmp(type, patch_type_str[kutf8]) == 0) {
            for (int i = 0; value[i] != '\0'; i++) {
                u8 val_ = value[i];
                u8 value_[1] = {val_};
                sys_proc_rw(pid, addr, value_, 1);
                addr++;
            }
            // null terminate string hack
            u8 value_[1] = {0};
            sys_proc_rw(pid, addr, value_, 1);
            return;
        } else if (strcmp(type, patch_type_str[kutf16]) == 0) {
            for (int i = 0; value[i] != '\x00'; i++) {
                u8 val_ = value[i];
                u8 value_[2] = {val_, 0x00};
                sys_proc_rw(pid, addr, value_, 2);
                addr = addr + 2;
            }
            // null terminate string hack
            u8 value_[2] = {0x00, 0x00};
            sys_proc_rw(pid, addr, value_, 2);
            return;
        } else {
            debug_printf("type not found or unsupported");
        }
    }
    return;
}

// http://www.cse.yorku.ca/~oz/hash.html
u64 hash(const char *str) {
    u64 hash = 5381;
    u32 c;
    while ((c = *str++))
        hash = hash * 33 ^ c;
    return hash;
}

u64 patch_hash_calc(const char *title, const char *name, const char *app_ver,
                    const char *title_id, const char *elf = "") {
    u64 output_hash = 0;
    char hash_str[256];
    snprintf(hash_str, sizeof(hash_str), "%s%s%s%s%s", title, name, app_ver,
             title_id, elf);
    output_hash = hash(hash_str);
    final_printf("input \"%s\"\n", hash_str);
    final_printf("output: 0x%016lx\n", output_hash);
    return output_hash;
}

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

int Write_File(const char *File, unsigned char *Data, size_t Size) {
    int32_t fd = sceKernelOpen(File, 0x200 | 0x002, 0777);
    if (fd < 0) {
        debug_printf("Failed to make file \"%s\"\n", File);
        return 0;
    }
    debug_printf("Writing File \"%s\" %li\n", File, Size);
    ssize_t written = sceKernelWrite(fd, Data, Size);
    debug_printf("Written File \"%s\" %li\n", File, written);
    sceKernelClose(fd);
    return 1;
}

u8 get_key_init() {
    patch_items = 0;
    char *buffer;
    u64 size;
    char input_file[64];
    snprintf(input_file, sizeof(input_file),
             "/data/GoldHEN/patches/json/%s.json", titleid);
    int res = Read_File(input_file, &buffer, &size, 32);
    if (res) {
        final_printf("file %s not found\n error: 0x%08x", input_file, res);
        return 1;
    }
    json_t mem[max_tokens];
    json_t const *json = json_create(buffer, mem, sizeof mem / sizeof *mem);
    if (!json) {
        Notify("Too many tokens or bad file\n");
        return 1;
    }

    json_t const *patchItems = json_getProperty(json, "patch");
    if (!patchItems || JSON_ARRAY != json_getType(patchItems)) {
        final_printf("patch not found\n");
        return 1;
    }

    json_t const *patches;
    for (patches = json_getChild(patchItems); patches != 0;
         patches = json_getSibling(patches)) {
        if (JSON_OBJ == json_getType(patches)) {
            char const *gameTitle = json_getPropertyValue(patches, key_title);
            if (gameTitle)
                final_printf("%s: %s\n", key_title, gameTitle);
            char const *gameAppver =
                json_getPropertyValue(patches, key_app_ver);
            if (gameAppver)
                final_printf("%s: %s\n", key_app_ver, gameAppver);
            char const *gameAppElf =
                json_getPropertyValue(patches, key_app_elf);
            if (gameAppElf)
                final_printf("%s: %s\n", key_app_elf, gameAppElf);
            char const *gameName = json_getPropertyValue(patches, key_name);
            if (gameName)
                final_printf("%s: %s\n", key_name, gameName);
            char const *gameAuthor = json_getPropertyValue(patches, key_author);
            if (gameAuthor)
                final_printf("%s: %s\n", key_author, gameAuthor);
            char const *gameNote = json_getPropertyValue(patches, key_note);
            if (gameNote)
                final_printf("%s: %s\n", key_note, gameNote);
            json_t const *patch_List_Items =
                json_getProperty(patches, key_patch_list);
            json_t const *patch_lists;
            u64 hashout = patch_hash_calc(  gameTitle,
                                            gameName,
                                            gameAppver,
                                            input_file,
                                            gameAppElf);
            for (patch_lists = json_getChild(patch_List_Items);
                 patch_lists != 0; patch_lists = json_getSibling(patch_lists)) {
                if (JSON_OBJ == json_getType(patch_lists)) {
                    u64 addr_real = 0;
                    char const *gameType =
                        json_getPropertyValue(patch_lists, key_app_type);
                    if (gameType) {
                        debug_printf("  %s: %s\n", key_app_type, gameType);
                    } else {
                        final_printf("  %s: not found\n", key_app_type);
                        return 1;
                    }
                    char const *gameAddr =
                        json_getPropertyValue(patch_lists, key_app_addr);
                    if (gameAddr) {
                        addr_real = strtoull(gameAddr, NULL, 16);
                        debug_printf("  %s: 0x%08lx\n", key_app_addr,
                                     addr_real);
                    } else {
                        final_printf("  %s: not found\n", key_app_addr);
                        return 1;
                    }
                    char const *gameValue =
                        json_getPropertyValue(patch_lists, key_app_value);
                    if (gameValue) {
                        debug_printf("  %s: %s\n", key_app_value, gameValue);
                    } else {
                        final_printf("  %s: not found\n", key_app_value);
                        return 1;
                    }
                    char settings_path[64];
                    snprintf(settings_path, sizeof(settings_path),
                             "/data/GoldHEN/patches/settings/0x%016lx.txt",
                             hashout);
                    char *buffer2;
                    u64 size2;
                    int res = Read_File(settings_path, &buffer2, &size2, 32);
                    if (res == 0x80020002) {
                        unsigned char test[2] = {0x30, 0xa};
                        Write_File(settings_path, test, sizeof(test));
                        res = 0;
                    } else if (res == 0) {
                        if (buffer2[0] == 0x31 &&
                            strcmp(game_elf, gameAppElf) == 0 &&
                            strcmp(game_ver, gameAppver) == 0) {
                            debug_printf("ver elf %s %s \n", game_ver, game_elf);
                            debug_printf("setting %s true\n", settings_path);
                            patch_data1(gameType, addr_real, gameValue);
                            patch_items++;
                        }
                    }
                    debug_printf("settings path: %s ret: 0x%08x\n",
                                 settings_path, res);
                }
            }
        }
    }
    if (patch_items == 0) {
        return 0;
    }
    if (patch_items == 1) {
        Notify("%li Patch Applied", patch_items);
    }
    if (patch_items > 1) {
        Notify("%li Patches Applied", patch_items);
    }
    return 0;
}

void make_folders(){
    if (mkdir(base_path, 0777) < 0) {
        final_printf("Creating %s failed, does it already exist?\n", base_path);
    }
    if (mkdir(base_path_patch, 0777) < 0) {
        final_printf("Creating %s failed, does it already exist?\n", base_path_patch);
    }
    if (mkdir(base_path_patch_json, 0777) < 0) {
        final_printf("Creating %s failed, does it already exist?\n", base_path_patch_json);
    }
    if (mkdir(base_path_patch_settings, 0777) < 0) {
        final_printf("Creating %s failed, does it already exist?\n", base_path_patch_settings);
    }
}

extern "C" {
int __attribute__((weak)) __attribute__((visibility("hidden")))
module_start(s64 argc, const void *args) {
    patch_items = 0;
    final_printf("[GoldHEN] module_start\n");
    make_folders();
    pid = 0;
    proc_info procInfo;
    if (!sys_sdk_proc_info(&procInfo)) {
        memcpy(titleid, procInfo.titleid, 16);
        memcpy(game_elf, procInfo.name, 32);
        memcpy(game_ver, procInfo.version, 8);
        final_printf("pid: %d\n", procInfo.pid);
        final_printf("executable name: %s\n", procInfo.name);
        final_printf("title id: %s\n", procInfo.titleid);
        final_printf("app version: %s\n", procInfo.version);
        final_printf("base address: 0x%lx\n", procInfo.base_address);
    }
    const char *gpudump_name = "gpudump.elf";
    pid = procInfo.pid;
    if (strcmp(procInfo.name, gpudump_name) == 0) {
        final_printf("Omitting %s\n", gpudump_name);
        return 0;
    }
    u8 ret = get_key_init();
    if (!ret) {
        final_printf("get_key_init() failed with 0x%02x\n", ret);
        return 1;
    }
    return 0;
}

int __attribute__((weak)) __attribute__((visibility("hidden")))
module_stop(s64 argc, const void *args) {
    final_printf("[GoldHEN] module_stop\n");
    return 0;
}
}
