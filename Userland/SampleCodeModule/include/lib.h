#ifndef LIB_H
#define LIB_H

#define BUFFER_SIZE 256
#define stdin 0 

#include <stdint.h>
#include <stddef.h>

void putchar(char c);
char getchar();
int scanf(const char *fmt, ...);
int printf(const char *fmt, ...);
int strlen(const char *s);
void strncpy(char *dest, const char *src, size_t n);
int atoi(char* str);

#endif