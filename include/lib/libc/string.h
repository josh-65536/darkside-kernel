#ifndef __STRING_H
#define __STRING_H

#include <lib/libc/types.h>

/* Memory functions */
uint8_t *memcpy(void *dest, void *src, int32_t count);
uint8_t *memset(void *dest, uint8_t val, int32_t count);
uint16_t *memsetw(uint16_t *dest, uint16_t val, int32_t count);
bool memequal(void *ptr1, void *ptr2, uint32_t count);
void memclr(void *address, uint32_t length);

/* String functions */
int32_t strlen(uint8_t *str);
uint8_t *strcpy(uint8_t *dest, uint8_t *src);
uint8_t *strncpy(uint8_t *dest, uint8_t *src, uint32_t size);
bool strequal(uint8_t *s1, uint8_t *s2);
bool strnequal(uint8_t *s1, uint8_t *s2, uint32_t num);
uint8_t *strlower(uint8_t *str);
uint8_t *strupper(uint8_t *str);
uint8_t *strcat(uint8_t *s1, uint8_t *s2);
uint8_t *strtok(uint8_t *str, uint8_t *delimeter, uint8_t **saveptr);

#endif
