#include <stdio.h>

#include <common.h>
#include <stdlib.h>
#include <string.h>

// if new_str len is greater than or equal to n, the char gets replaced like so:
// new_str[n - 1] = '\0';
Sac_String new_sac_string(const char *new_str, size_t n) {
  unsigned long long char_count = strlen(new_str);
  Sac_String str;
  str.is_statically_allocated = true;
  str.alloc_err = false;

  if (char_count <= n) {
    str.chars = new_str;
    str.length = char_count;
    return str;
  }

  str = alloc_sac_string(new_str, n);
  return str;
}

Sac_String default_new_sac_string(const char *new_str) {
  return new_sac_string(new_str, BYTE_1024);
};

Sac_String alloc_sac_string(const char *new_str, size_t n) {
  Sac_String str;
  str.is_statically_allocated = false;
  str.alloc_err = false;

  char *tmp = n > 0
    ? malloc(sizeof(char) * n)
    : NULL;

  if (tmp == NULL) {
    str.length = 0;
    str.chars = NULL_STR;
    str.is_statically_allocated = true;
    str.alloc_err = true;
    return str;
  }

  sac_strncpy(tmp, new_str, n);
  str.length = n - 1;
  str.chars = tmp;

  return str;
}

char *sac_strncpy(char *dest, const char *target, size_t n) {
  if (n == 0) return dest;
  strncpy(dest, target, n);
  dest[n - 1] = '\0';
  return dest;
}
