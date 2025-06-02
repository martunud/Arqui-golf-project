#include "include/lib.h"
#include "include/game.h"

#define COLOR_BG_GREY 0xD3D3D3
#define COLOR_TEXT_BLUE 0x0000FF
#define COLOR_BG_GREEN 0x2ECC40
#define COLOR_PLAYER 0x0000FF
#define COLOR_BALL 0xFFFF00
#define COLOR_HOLE 0x000000
#define COLOR_TEXT_WHITE 0xFFFFFF

void drawCircle(int cx, int cy, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                video_putPixel(cx + x, cy + y, color);
            }
        }
    }
}

void drawText(int x, int y, const char *text, uint32_t color) {
    // Mueve el cursor a la posición deseada (solo aproximado)
    int lines = y / 16; // Suponiendo 16px de alto por letra
    int spaces = x / 8; // Suponiendo 8px de ancho por letra
    for (int i = 0; i < lines; i++) video_putChar('\n', color, COLOR_BG_GREY);
    for (int i = 0; i < spaces; i++) video_putChar(' ', color, COLOR_BG_GREY);
    while (*text) {
        video_putChar(*text, color, COLOR_BG_GREY);
        text++;
    }
}

void game_main_screen() {
    video_clearScreenColor(COLOR_BG_GREY);
    int centerX = 400, centerY = 300; // Ajustar para mejor centrado
    
    drawText(centerX - 120, centerY - 50, "Bienvenido a Pongis-Golf", COLOR_TEXT_BLUE);
    drawText(centerX - 100, centerY, "Presione 1 para empezar", COLOR_TEXT_BLUE);
    drawText(centerX - 80, centerY + 50, "Presione ESC para salir", COLOR_TEXT_BLUE);

    char input;
    while (1) {
        input = getchar(); // Usar syscall para leer input
        if (input == '1') {
            game_start();
            break;
        } else if (input == 27) { // ESC
            // Limpiar pantalla antes de salir
            clearScreen(); // Esto usa el color de fondo negro del shell
            return; // Volver al shell
        }
    }
}

void game_start() {
    video_clearScreenColor(COLOR_BG_GREEN);
    
    // Posiciones iniciales
    int ball_x = 50, ball_y = 300;
    int hole_x = 700, hole_y = 300;
    int power = 0;
    
    while (1) {
        // Limpiar área de juego
        video_clearScreenColor(COLOR_BG_GREEN);
        
        // Dibujar elementos del juego
        drawCircle(ball_x, ball_y, 5, COLOR_BALL);
        drawCircle(hole_x, hole_y, 15, COLOR_HOLE);
        
        // Mostrar información
        char power_str[20];
        sprintf(power_str, "Power: %d", power);
        drawText(10, 10, power_str, COLOR_TEXT_WHITE);
        drawText(10, 30, "Usa W/S para ajustar power", COLOR_TEXT_WHITE);
        drawText(10, 50, "ESC para salir", COLOR_TEXT_WHITE);
        
        char input = getchar();
        if (input == 27) { // ESC para salir
            // Limpiar pantalla antes de salir del juego
            clearScreen(); // Esto usa el color de fondo negro del shell
            break;
        }
        else if (input == 'w' || input == 'W') {
            if (power < 100) power += 5;
        }
        else if (input == 's' || input == 'S') {
            if (power > 0) power -= 5;
        }
        // Agregar más lógica de juego aquí
    }
}