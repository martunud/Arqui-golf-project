#include "../include/videoDriver.h"
#include "../include/font.h"

static int isSpecialChar(char c);

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));


VBEinfoPtr VBEModeInfo = (VBEinfoPtr) 0x0000000000005C00;

//Variables para la posicion
uint64_t cursorX = 0;
uint64_t cursorY = 0;


void video_putPixel(uint32_t color, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *)(uintptr_t) VBEModeInfo->framebuffer;
    uint64_t offset = (x * ((VBEModeInfo->bpp)/8)) + (y * VBEModeInfo->pitch);
    framebuffer[offset]     =  (color) & 0xFF;
    framebuffer[offset+1]   =  (color >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (color >> 16) & 0xFF;
}

void video_putChar(char c, uint64_t foregroundColor, uint64_t backgroundColor) {
    //veridica caracter especial
    if(isSpecialChar(c)){
        return;
    }

    // Obtener el índice del carácter en la fuente
    uint8_t charIndex = (uint8_t)c;
    
    // Obtener el patrón de bits del carácter
    unsigned char* charPattern = font8x16[charIndex];
    
    // Dibujar el carácter pixel por pixel
    for (int y = 0; y < FONT_HEIGHT; y++) {
        for (int x = 0; x < FONT_WIDTH; x++) {
            // Verificar si el bit está activo en el patrón
            if (charPattern[y] & (1 << (7 - x))) {
                // Si el bit está activo, dibujar el pixel con el color de primer plano
                video_putPixel(foregroundColor, cursorX + x, cursorY + y);
            } else {
                // Si el bit está inactivo, dibujar el pixel con el color de fondo
                video_putPixel(backgroundColor, cursorX + x, cursorY + y);
            }
        }
    }
    //Actualizar la posicion
    cursorX += FONT_WIDTH;
    if(cursorX >= VBEModeInfo->width) {
        video_newLine();
    }
}

static int isSpecialChar(char c){
        switch(c) {
            case '\n':
                video_newLine();
                return 1;
            case '\b':
                video_backSpace();
                return 1;
            case '\t':
                video_tab();
                return 1;
        }
        return 0;
}

void video_clearScreen() {
    for(int i=0; i<VBEModeInfo->height; i++) {
        for(int j=0; j<VBEModeInfo->width; j++) {
            video_putPixel(BACKGROUND_COLOR, j, i);
        }
    }
    cursorX = 0;
    cursorY = 0;
}

void video_putString(char *string, uint64_t foregroundColor, uint64_t backgroundColor) {
    while(*string) {
        video_putChar(*string, foregroundColor, backgroundColor);
        string++;
    }
}

void video_newLine(){
    cursorX = 0;
    cursorY += FONT_HEIGHT;
}

//Hay que arreglar para cuando borramos el primer caracter de la linea

void video_backSpace(){
    if(cursorX > 0){
        cursorX -= FONT_WIDTH;
        for(int i = 0; i < FONT_WIDTH; i++){
            for(int j = 0; j < FONT_HEIGHT; j++){
                video_putPixel(BACKGROUND_COLOR, cursorX + i, cursorY + j);
            }
        }
    }
}

void video_tab(){
    uint64_t step = FONT_WIDTH * 8;
    cursorX = (cursorX / (step) + 1) * (step);
    if(cursorX > VBEModeInfo->width){
        video_newLine();
    }
}

// static void clearLine()


// void video_scrollUp(){

// }