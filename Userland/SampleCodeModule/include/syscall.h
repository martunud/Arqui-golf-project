#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

uint64_t sys_read(int fd, char * buffer, int count);
uint64_t sys_write(int fd, const char * buffer, int count);
uint64_t sys_getRegisters(uint64_t *buffer);
uint64_t sys_getTime(uint8_t reg);
uint64_t sys_clearScreen();
uint64_t sys_setFontScale(int scale);
uint64_t sys_sleep(int duration);
uint64_t sys_beep(int frequency, int duration);
#endif