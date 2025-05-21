#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

#define BACKGROUND_COLOR 0x000000
#define FOREGROUND_COLOR 0xFFFFFF


typedef struct vbe_mode_info_structure *VBEinfoPtr;

void video_putPixel(uint32_t color, uint64_t x, uint64_t y);

void video_putChar(char c, uint64_t foregroundColor, uint64_t backgroundColor);

void video_clearScreen();

void video_putString(char *string, uint64_t foregroundColor, uint64_t backgroundColor);

void video_newLine();

void video_backSpace();

void video_tab();


#endif