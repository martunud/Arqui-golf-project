#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

uint64_t sys_read(int fd, char * buffer, int count);
uint64_t sys_write(int fd, const char * buffer, int count);
uint64_t sys_getTime(uint8_t reg);
uint64_t sys_clearScreen();
uint64_t sys_setFontScale(int scale);
uint64_t sys_sleep(int duration);
uint64_t sys_beep(int frequency, int duration);

uint64_t sys_video_putPixel(int x, int y, uint32_t color);
uint64_t sys_video_putChar(char c, uint32_t fg, uint32_t bg);
uint64_t sys_video_clearScreenColor(uint32_t color);
uint64_t sys_video_putCharXY(char c, int x, int y, uint32_t fg, uint32_t bg);
uint64_t sys_regs(void *user_buf);
uint64_t sys_is_key_pressed(unsigned char scancode);
void sys_shutdown();

#endif