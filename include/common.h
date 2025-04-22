#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <stdint.h>

#define BYTE_256 256
#define BYTE_512 512

#define HIGH_ORDER_255 0xff

typedef struct {
  float width;
  float height;
} Sac_Dimensions;

typedef struct {
    int32_t length;
    const char *chars;
    const char *baseChars; // The source string / char* that this slice was derived from
} Sac_StringSlice;

#endif
