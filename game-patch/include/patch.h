enum patch_types
{
    kbyte = 0,
    kbytes16,
    kbytes32,
    kbytes64,
    kbytes,
    kfloat32,
    kfloat64,
    kutf8,
    kutf16,
    type_max
};

static const char* patch_type_str[] =
{
    [patch_types::kbyte]  = "byte",
    [patch_types::kbytes16]  = "bytes16",
    [patch_types::kbytes32]  = "bytes32",
    [patch_types::kbytes64]  = "bytes64",
    [patch_types::kbytes]  = "bytes",
    [patch_types::kfloat32]  = "float32",
    [patch_types::kfloat64]  = "float64",
    [patch_types::kutf8]  = "utf8",
    [patch_types::kutf16]  = "utf16",
};
