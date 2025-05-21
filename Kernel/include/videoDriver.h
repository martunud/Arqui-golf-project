#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

typedef struct vbe_mode_info_structure *VBEinfoPtr;

void video_putPixel(uint32_t color, uint64_t x, uint64_t y);

void video_putChar(char c, uint64_t foregroundColor, uint64_t backgroundColor);

#endif