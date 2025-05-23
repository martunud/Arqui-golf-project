#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

uint64_t sys_read(int fd, char * buffer, int count);
uint64_t sys_write(int fd, const char * buffer, int count);
uint64_t sys_getRegisters(uint64_t *buffer);
uint64_t sys_getTime(char *buffer);
uint64_t sys_clearScreen();
uint64_t sys_setFontScale(int scale);

#endif