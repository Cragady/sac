#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#define NULL_STR ""

#include <stdbool.h>
#include <stdint.h>

#define BYTE_256 256
#define BYTE_512 512
#define BYTE_1024 1024

#define HIGH_ORDER_255 0xff

#define SAC_STRING(str) default_new_sac_string(str)

typedef struct {
  float width;
  float height;
} Sac_Dimensions;

typedef struct {
    int32_t length;
    const char *chars;
    const char *base_chars; // The source string / char* that this slice was derived from
} Sac_StringSlice;

typedef struct {
    bool is_statically_allocated;
    bool alloc_err;
    int32_t length;
    const char *chars;
} Sac_String;

Sac_String new_sac_string(const char *new_str, size_t n);
Sac_String default_new_sac_string(const char *new_str);
char *sac_strncpy(char *dest, const char *target, size_t n);
Sac_String alloc_sac_string(const char *new_str, size_t n);
void free_sac_string(Sac_String *str);

#endif
