#include "../include/videoDriver.h"
#include "../include/font.h"

static int isSpecialChar(char c);
static void video_scrollUp();

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
    if (cursorY + FONT_HEIGHT < VBEModeInfo->height) {
        cursorY += FONT_HEIGHT;
    } else {
        video_scrollUp();
        cursorY = VBEModeInfo->height - FONT_HEIGHT;
    }
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
    } else if(cursorY > 0) {
        // Subir a la línea anterior
        cursorY -= FONT_HEIGHT;
        cursorX = VBEModeInfo->width - FONT_WIDTH;
        // Buscar el último carácter no vacío de la línea anterior
        // Opcional: puedes mejorar esto si tienes un buffer de texto
        for(; cursorX >= 0; cursorX -= FONT_WIDTH) {
            int empty = 1;
            for(int i = 0; i < FONT_WIDTH && empty; i++){
                for(int j = 0; j < FONT_HEIGHT && empty; j++){
                    // Verifica si el pixel es diferente al color de fondo
                    uint8_t * framebuffer = (uint8_t *)(uintptr_t) VBEModeInfo->framebuffer;
                    uint64_t offset = ((cursorX + i) * ((VBEModeInfo->bpp)/8)) + ((cursorY + j) * VBEModeInfo->pitch);
                    if(framebuffer[offset] != (BACKGROUND_COLOR & 0xFF) ||
                       framebuffer[offset+1] != ((BACKGROUND_COLOR >> 8) & 0xFF) ||
                       framebuffer[offset+2] != ((BACKGROUND_COLOR >> 16) & 0xFF)) {
                        empty = 0;
                    }
                }
            }
            if(!empty) {
                // Borra el carácter encontrado
                for(int i = 0; i < FONT_WIDTH; i++){
                    for(int j = 0; j < FONT_HEIGHT; j++){
                        video_putPixel(BACKGROUND_COLOR, cursorX + i, cursorY + j);
                    }
                }
                break;
            }
            if(cursorX == 0) break;
        }
        // Ajusta cursorX para el siguiente backspace
        if(cursorX < 0) cursorX = 0;
    }
}

void video_tab(){
    uint64_t step = FONT_WIDTH * 8;
    cursorX = (cursorX / (step) + 1) * (step);
    if(cursorX > VBEModeInfo->width){
        video_newLine();
    }
}

void video_moveCursorLeft() {
    if(cursorX >= FONT_WIDTH) {
        cursorX -= FONT_WIDTH;
    } else if(cursorY >= FONT_HEIGHT) {
        cursorY -= FONT_HEIGHT;
        cursorX = VBEModeInfo->width - FONT_WIDTH;
    }
}

void video_moveCursorRight() {
    if(cursorX + FONT_WIDTH < VBEModeInfo->width) {
        cursorX += FONT_WIDTH;
    } else if(cursorY + FONT_HEIGHT < VBEModeInfo->height) {
        cursorX = 0;
        cursorY += FONT_HEIGHT;
    }
}

void video_moveCursorUp() {
    if(cursorY >= FONT_HEIGHT) {
        cursorY -= FONT_HEIGHT;
    }
}

void video_moveCursorDown() {
    if(cursorY + FONT_HEIGHT < VBEModeInfo->height) {
        cursorY += FONT_HEIGHT;
    }
}

void video_drawCursor(uint64_t color) {
    for (int x = 0; x < FONT_WIDTH; x++) {
        video_putPixel(color, cursorX + x, cursorY + FONT_HEIGHT - 2);
    }
}


static void video_scrollUp() {
    uint8_t *fb = (uint8_t *)(unsigned long)VBEModeInfo->framebuffer;
    int row_bytes   = VBEModeInfo->pitch;
    int n_rows      = VBEModeInfo->height;
    int move_bytes  = (n_rows - FONT_HEIGHT) * row_bytes; //todas las filas menos la primera
    uint8_t *src = fb + FONT_HEIGHT * row_bytes; //primer byte debajo de la primera linea
    uint8_t *dst = fb; //primer byte de la pantalla

    // haog la copia de las lineas
    for (int i = 0; i < move_bytes; i++) {
        dst[i] = src[i];
    }

    // borro lo que no iba a entrar 
    for (int y = n_rows - FONT_HEIGHT; y < n_rows; y++) {
        for (int x = 0; x < VBEModeInfo->width; x++) {
            video_putPixel(BACKGROUND_COLOR, x, y);
        }
    }
}
