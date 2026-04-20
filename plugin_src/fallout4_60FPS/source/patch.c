#include <string.h>
#include <Common.h>
#include "patch.h"
#include "plugin_common.h"

/////////////////////////////////////////////////////////////
//                    Pattern Matching                     //
/////////////////////////////////////////////////////////////

uint32_t pattern_to_byte(const char* pattern, uint8_t* bytes) {
    uint32_t count = 0;
    const char* start = pattern;
    const char* end = pattern + strlen(pattern);

    for (const char* current = start; current < end; ++current) {
        if (*current == '?') {
            ++current;
            if (*current == '?') {
                ++current;
            }
            bytes[count++] = 0xff;
        } else {
            bytes[count++] = (uint8_t)strtoul(current, (char**)&current, 16);
        }
    }
    return count;
}

uint8_t* PatternScan(uint64_t module_base, uint32_t module_size, const char* signature) {
    if (!module_base || !module_size) {
        return NULL;
    }
    
    enum { MAX_PATTERN_LENGTH = 256 };
    uint8_t patternBytes[MAX_PATTERN_LENGTH] = { 0 };
    int32_t patternLength = (int32_t)pattern_to_byte(signature, patternBytes);
    
    if (!patternLength || patternLength >= MAX_PATTERN_LENGTH) {
        final_printf("Pattern length too large or invalid! %i\n", patternLength);
        return NULL;
    }
    if ((uint32_t)patternLength > module_size) {
        return NULL;
    }
    
    uint8_t* scanBytes = (uint8_t*)module_base;
    uint32_t maxIndex = module_size - (uint32_t)patternLength;

    for (uint32_t i = 0; i <= maxIndex; ++i) {
        int found = 1;
        for (int32_t j = 0; j < patternLength; ++j) {
            if (scanBytes[i + j] != patternBytes[j] && patternBytes[j] != 0xff) {
                found = 0;
                break;
            }
        }
        if (found) {
            return &scanBytes[i];
        }
    }
    return NULL;
}

/////////////////////////////////////////////////////////////
//                   Patch Application                     //
/////////////////////////////////////////////////////////////

int32_t apply_patch(uint64_t module_base, uint32_t module_size, const Patch* patch) {
    if (!patch || !patch->data) {
        return -1;
    }

    // Direct address patch (pattern is NULL)
    if (!patch->pattern) {
        if (!patch->direct_addresses || patch->address_count == 0) {
            return -1;
        }

        for (uint32_t i = 0; i < patch->address_count; ++i) {
            uint64_t addr = patch->direct_addresses[i];
            
            // Protect the memory page for writing
            uintptr_t page = addr & ~0xfff;
            sceKernelMprotect((void*)page, 0x1000, VM_PROT_ALL);
            
            // Apply the patch
            memcpy((void*)addr, patch->data, patch->data_size);
            
            final_printf("Applied %s patch at address 0x%lx\n", 
                         patch->name, addr);
        }

        return (int32_t)patch->address_count;
    }

    // Pattern-based patch
    uint8_t* addr = (uint8_t*)module_base;
    uint32_t occurrence = 0;
    
    while (addr < (uint8_t*)(module_base + module_size)) {
        addr = PatternScan((uint64_t)addr, module_size - (addr - (uint8_t*)module_base), patch->pattern);
        if (!addr) break;
        
        if (patch->max_occurrences > 0 && occurrence >= patch->max_occurrences) {
            break;
        }

        // Protect the memory page for writing
        uintptr_t page = (uintptr_t)(addr + patch->offset) & ~0xfff;
        sceKernelMprotect((void*)page, 0x1000, VM_PROT_ALL);
        
        // Apply the patch
        memcpy(addr + patch->offset, patch->data, patch->data_size);
        occurrence++;
        
        final_printf("Applied %s patch at offset 0x%lx (occurrence #%u)\n", 
                     patch->name, (uint64_t)addr - module_base, occurrence);
        
        addr++;
    }

    if (occurrence == 0) {
        Notify(TEX_ICON_SYSTEM, "%s pattern not found, are you on version 1.43?\n", patch->name);
        return -1;
    }
    
    return (int32_t)occurrence;
}
