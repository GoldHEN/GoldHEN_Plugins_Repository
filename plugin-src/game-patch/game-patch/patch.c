#include "patch.h"

s32 pid = 0;
u8 arr8[1];
u8 arr16[2];
u8 arr32[4];
u8 arr64[8];

const char *hex_prefix = "0x";

unsigned char *hexstrtochar2(const char *hexstr, s64 *size) {
    // valid hex look up table.
    const u8 hex_lut[] = {
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

    u32 str_len = strlen(hexstr);
    s64 data_len = ((str_len + 1) / 2) * sizeof(unsigned char);

    *size = (str_len) * sizeof(unsigned char);

    unsigned char *data = (unsigned char *)malloc(*size);
    u32 j = 0; // hexstr position
    u32 i = 0; // data position

    if (str_len % 2 == 1) {
        data[i] = (u8)(hex_lut[0] << 4) | hex_lut[(u8)hexstr[j]];
        j = ++i;
    }

    for (; j < str_len; j += 2, i++) {
        data[i] = (u8)(hex_lut[(u8)hexstr[j]] << 4) |
                  hex_lut[(u8)hexstr[j + 1]];
    }

    *size = data_len;
    return data;
}

void sys_proc_rw(u64 address, void *data, u64 length) {
    struct proc_rw process_rw_data;
    process_rw_data.address = address;
    process_rw_data.data = data;
    process_rw_data.length = length;
    process_rw_data.write_flags = 1;
    #if(__FINAL__) == 0
    debug_printf("process_rw_data: %p\n", &process_rw_data);
    debug_printf("address: 0x%lx\n", process_rw_data.address);
    debug_printf("hex_dump: ");
    hex_dump(process_rw_data.data, process_rw_data.length);
    debug_printf("length: 0x%lx\n", process_rw_data.length);
    #endif
    sys_sdk_proc_rw(&process_rw_data);
    return;
}

// https://stackoverflow.com/a/4770992
bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
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
                    const char *title_id, const char *elf) {
    u64 output_hash = 0;
    char hash_str[256];
    snprintf(hash_str, sizeof(hash_str), "%s%s%s%s%s", title, name, app_ver,
             title_id, elf);
    output_hash = hash(hash_str);
    debug_printf("input: \"%s\"\n", hash_str);
    debug_printf("output: 0x%016lx\n", output_hash);
    return output_hash;
}

void patch_data1(const char *type, u64 addr, const char *value) {
    if (type) {
        s32 str_base = 16;
        memset(arr8,  0, sizeof(arr8));
        memset(arr16, 0, sizeof(arr16));
        memset(arr32, 0, sizeof(arr32));
        memset(arr64, 0, sizeof(arr64));
        if (strcmp(type, "byte") == 0) {
            u8 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtol(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtol(value, NULL, str_base);
            }
            memcpy(arr8, &real_value, sizeof(arr8));
            sys_proc_rw(addr, arr8, sizeof(arr8));
            return;
        } else if (strcmp(type, "bytes16") == 0) {
            u16 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtol(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtol(value, NULL, str_base);
            }
            memcpy(arr16, &real_value, sizeof(arr16));
            sys_proc_rw(addr, arr16, sizeof(arr16));
            return;
        } else if (strcmp(type, "bytes32") == 0) {
            u32 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtol(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtol(value, NULL, str_base);
            }
            memcpy(arr32, &real_value, sizeof(arr32));
            sys_proc_rw(addr, arr32, sizeof(arr32));
            return;
        } else if (strcmp(type, "bytes64") == 0) {
            s64 real_value = 0;
            if (prefix(hex_prefix, value)) {
                real_value = strtoll(value, NULL, str_base);
            } else {
                str_base = 10;
                real_value = strtoll(value, NULL, str_base);
            }
            memcpy(arr64, &real_value, sizeof(arr64));
            sys_proc_rw(addr, arr64, sizeof(arr64));
            return;
        } else if (strcmp(type, "bytes") == 0) {
            s64 szb = 0;
            u8 *bytearray = hexstrtochar2(value, &szb);
            sys_proc_rw(addr, bytearray, szb);
            return;
        } else if (strcmp(type, "float32") == 0) {
            f32 real_value = 0;
            real_value = strtod(value, NULL);
            memcpy(arr32, &real_value, sizeof(arr32));
            sys_proc_rw(addr, arr32, sizeof(arr32));
            return;
        } else if (strcmp(type, "float64") == 0) {
            f64 real_value = 0;
            real_value = strtod(value, NULL);
            memcpy(arr64, &real_value, sizeof(arr64));
            sys_proc_rw(addr, arr64, sizeof(arr64));
            return;
        } else if (strcmp(type, "utf8") == 0) {
            for (s32 i = 0; value[i] != '\0'; i++) {
                u8 val_ = value[i];
                u8 value_[1] = {val_};
                sys_proc_rw(addr, value_, sizeof(value_));
                addr++;
            }
            // null terminate string hack
            u8 value_[1] = { 0x00 };
            sys_proc_rw(addr, value_, sizeof(value_));
            return;
        } else if (strcmp(type, "utf16") == 0) {
            for (s32 i = 0; value[i] != '\x00'; i++) {
                u8 val_ = value[i];
                u8 value_[2] = {val_, 0x00};
                sys_proc_rw(addr, value_, sizeof(value_));
                addr = addr + 2;
            }
            // null terminate string hack
            u8 value_[2] = {0x00, 0x00};
            sys_proc_rw(addr, value_, sizeof(value_));
            return;
        } else {
            final_printf("type not found or unsupported\n");
        }
    }
    return;
}
