#include "include/game.h"
#include "include/lib.h"
#include "include/syscall.h"

// Dibuja un círculo relleno (jugador, pelota, agujero)
void drawCircle(int cx, int cy, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                video_putPixel(cx + x, cy + y, color);
            }
        }
    }
}

// Dibuja el texto del nivel (ejemplo simple)
void drawText(int x, int y, const char *text, uint32_t color) {
    // Si tienes video_putChar, úsalo. Si no, implementa un print básico.
    while (*text) {
        video_putChar(*text, color, 0x000000); // fondo negro
        x += 8; // avanza 8 píxeles (ancho de letra)
        text++;
    }
}

// Dibuja el jugador (azul), la pelota (amarilla) y el agujero (negro)
void drawGameObjects(int playerX, int playerY, int ballX, int ballY, int holeX, int holeY, int level) {
    drawCircle(playerX, playerY, 20, 0x0000FF); // Azul
    drawCircle(ballX, ballY, 10, 0xFFFF00);     // Amarillo
    drawCircle(holeX, holeY, 15, 0x000000);     // Negro

    char levelText[16];
    sprintf(levelText, "HOLE %d", level);
    drawText(holeX + 20, holeY - 10, levelText, 0xFFFFFF); // Blanco
}