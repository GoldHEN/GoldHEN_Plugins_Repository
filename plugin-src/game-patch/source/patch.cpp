#include "patch.h"

int pid = 0;
u8 arr8[1];
u8 arr16[2];
u8 arr32[4];
u8 arr64[8];

const char *hex_prefix = "0x";

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

int sys_proc_rw(uint64_t pid, uint64_t address, void *data, uint64_t length, uint64_t write) {
    debug_printf("writing to memory at 0x%lx\n", address);
    return orbis_syscall(108 + 90, pid, address, data, length, write);
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
    final_printf("input: \"%s\"\n", hash_str);
    final_printf("output: 0x%016lx\n", output_hash);
    return output_hash;
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
            final_printf("type: %s unsupported\n", type);
            // strtod, atof crashes
            return;
        } else if (strcmp(type, patch_type_str[kfloat64]) == 0) {
            final_printf("type: %s unsupported\n", type);
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
            final_printf("type not found or unsupported\n");
        }
    }
    return;
}
