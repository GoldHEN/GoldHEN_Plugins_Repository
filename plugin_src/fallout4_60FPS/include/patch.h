#ifndef PATCH_H
#define PATCH_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    const char* name;              // Name of the patch
    const char* pattern;           // Pattern to scan for (NULL for direct address patches)
    const uint8_t* data;           // Patch data to write
    uint32_t data_size;            // Size of patch data
    uint32_t offset;               // Offset from pattern start (or for direct, offset within data)
    uint32_t max_occurrences;      // Max occurrences to patch (0 = all, only for pattern-based)
    
    // Direct address patch fields (set pattern to NULL for direct patches)
    const uint64_t* direct_addresses;  // Array of addresses to write to
    uint32_t address_count;            // Number of addresses
} Patch;

// Helper functions
uint32_t pattern_to_byte(const char* pattern, uint8_t* bytes);
uint8_t* PatternScan(uint64_t module_base, uint32_t module_size, const char* signature);

// Patch application (handles both pattern-based and direct address patches)
int32_t apply_patch(uint64_t module_base, uint32_t module_size, const Patch* patch);

#endif // PATCH_H
