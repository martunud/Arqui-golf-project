#ifndef LIB_H
#define LIB_H

#define BUFFER_SIZE 256
#define stdin 0 
#define REGISTERS_CANT 18

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rbp, rdi, rsi, r8, r9, r10, r11, r12, r13, r14, r15, rip, rsp, rflags;
} CPURegisters;


void putchar(char c);
char getchar();
int scanf(const char *fmt, ...);
int printf(const char *fmt, ...);
int strlen(const char *s);
void strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int atoi(char* str);
char *fgets(char *s, int n, int fd);
void clearScreen();
int sprintf(char *str, const char *fmt, ...);
int try_getchar(char *c);
void getTime(char *buffer);
void setFontScale(int scale);
void save_registers_snapshot(uint64_t *buffer);
void printHex64(uint64_t value);

void video_putPixel(int x, int y, uint32_t color);
void video_putChar(char c, uint32_t fg, uint32_t bg);
void video_clearScreenColor(uint32_t color); // Nueva función para limpiar con color
void sleep(int milliseconds);
void video_putCharXY(int x, int y, char c, uint32_t fg, uint32_t bg);
int get_regs(uint64_t *r);

#endif