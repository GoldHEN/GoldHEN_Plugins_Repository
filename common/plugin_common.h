#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define s8  int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#ifdef FINAL
#define debug_printf(MESSAGE,args...) { }
#else
#define debug_printf(MESSAGE,args...) { \
  const char *A[] = {MESSAGE}; \
  printf("%s: (%s) %d", __FILE__, __FUNCTION__, __LINE__); fflush(stdout);\
  if(sizeof(A) > 0) {\
    printf(": "); \
    printf(*A,##args); \
  } else {\
    printf("\n"); \
  }\
}
#endif
#define final_printf(MESSAGE,args...) { \
  const char *A[] = {MESSAGE}; \
  printf("%s: %d", __FILE__, __LINE__); fflush(stdout);\
  if(sizeof(A) > 0) {\
    printf(": "); \
    printf(*A,##args); \
  } else {\
    printf("\n"); \
  }\
}
