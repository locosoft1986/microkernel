#ifndef __STRING_H
#define __STRING_H

#include "stdint.h"

void *memcpy(void *dest, const void *src, int32_t count);
void *memset(void *dest, uint8_t val, int32_t count);
uint16_t *memsetw(uint16_t *dest, uint16_t val, int32_t count);

int32_t strlen(const int8_t *str);
int8_t *strcpy(int8_t *dest, const int8_t *src);
int8_t *strchr(const int8_t *str, int8_t c);
int8_t *strcat(int8_t *dest, const int8_t *src);
int32_t strcmp(const int8_t *s1, const int8_t *s2);

void putch(const int8_t c);
void puts(const int8_t *str);
void kprintf(int8_t *format,...);

#endif
