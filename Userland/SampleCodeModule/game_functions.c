#include "include/lib.h"
#include "include/game_functions.h"

const int cos_table[36] = {100,98,94,87,77,64,50,34,17,0,-17,-34,-50,-64,-77,-87,-94,-98,-100,-98,-94,-87,-77,-64,-50,-34,-17,0,17,34,50,64,77,87,94,98};
const int sin_table[36] = {0,17,34,50,64,77,87,94,98,100,98,94,87,77,64,50,34,17,0,-17,-34,-50,-64,-77,-87,-94,-98,-100,-98,-94,-87,-77,-64,-50,-34,-17};

// Dibuja un círculo
void drawCircle(int cx, int cy, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    video_putPixel(px, py, color);
                }
            }
        }
    }
}

// Dibuja texto en pantalla
void drawText(int x, int y, const char *text, uint32_t color) {
    for (int i = 0; text[i] != '\0'; i++) {
        video_putCharXY(x + i * 8, y, text[i], color, COLOR_BG_GREY);
    }
}

// Dibuja texto con fondo
void drawTextWithBg(int x, int y, const char *text, uint32_t textColor, uint32_t bgColor) {
    int lines = y / 16;
    int spaces = x / 8;
    for (int i = 0; i < lines; i++) video_putChar('\n', textColor, bgColor);
    for (int i = 0; i < spaces; i++) video_putChar(' ', textColor, bgColor);
    while (*text) {
        video_putChar(*text, textColor, bgColor);
        text++;
    }
}

// Borra la pelota anterior de forma inteligente
void eraseBallSmart(int prev_ball_x, int prev_ball_y, Player *players, int num_players, int hole_x, int hole_y) {
    for (int y = -5; y <= 5; y++) {
        for (int x = -5; x <= 5; x++) {
            if (x*x + y*y <= 5*5) {
                int px = prev_ball_x + x;
                int py = prev_ball_y + y;
                if (px >= 0 && px < SCREEN_WIDTH && py >= UI_TOP_MARGIN && py < SCREEN_HEIGHT) {
                    int painted = 0;
                    // Repinta jugador si corresponde
                    for (int j = 0; j < num_players; j++) {
                        if ((px - players[j].x)*(px - players[j].x) + (py - players[j].y)*(py - players[j].y) <= PLAYER_RADIUS*PLAYER_RADIUS) {
                            video_putPixel(px, py, players[j].color);
                            painted = 1;
                            break;
                        }
                    }
                    // Repinta hoyo si corresponde
                    if (!painted && ((px - hole_x)*(px - hole_x) + (py - hole_y)*(py - hole_y) <= 15*15)) {
                        video_putPixel(px, py, COLOR_HOLE);
                        painted = 1;
                    }
                    // Si no hay nada, pinta fondo
                    if (!painted) {
                        video_putPixel(px, py, COLOR_BG_GREEN);
                    }
                }
            }
        }
    }
}

// Borra el jugador anterior de forma inteligente
void erasePlayerSmart(int prev_x, int prev_y, Player *players, int num_players, int hole_x, int hole_y) {
    for (int y = -PLAYER_RADIUS; y <= PLAYER_RADIUS; y++) {
        for (int x = -PLAYER_RADIUS; x <= PLAYER_RADIUS; x++) {
            if (x*x + y*y <= PLAYER_RADIUS*PLAYER_RADIUS) {
                int px = prev_x + x;
                int py = prev_y + y;
                if (px >= 0 && px < SCREEN_WIDTH && py >= UI_TOP_MARGIN && py < SCREEN_HEIGHT) {
                    int painted = 0;
                    // Repinta otro jugador si corresponde
                    for (int j = 0; j < num_players; j++) {
                        if (prev_x != players[j].x || prev_y != players[j].y) {
                            if ((px - players[j].x)*(px - players[j].x) + (py - players[j].y)*(py - players[j].y) <= PLAYER_RADIUS*PLAYER_RADIUS) {
                                video_putPixel(px, py, players[j].color);
                                painted = 1;
                                break;
                            }
                        }
                    }
                    // Repinta pelota si corresponde
                    if (!painted) {
                        for (int j = 0; j < num_players; j++) {
                            if ((px - players[j].ball_x)*(px - players[j].ball_x) + (py - players[j].ball_y)*(py - players[j].ball_y) <= 5*5) {
                                video_putPixel(px, py, players[j].ball_color);
                                painted = 1;
                                break;
                            }
                        }
                    }
                    // Repinta hoyo si corresponde
                    if (!painted && ((px - hole_x)*(px - hole_x) + (py - hole_y)*(py - hole_y) <= 15*15)) {
                        video_putPixel(px, py, COLOR_HOLE);
                        painted = 1;
                    }
                    // Si no hay nada, pinta fondo
                    if (!painted) {
                        video_putPixel(px, py, COLOR_BG_GREEN);
                    }
                }
            }
        }
    }
}

// Devuelve 1 si el punto está dentro del hoyo
int isInsideHole(int x, int y, int h_x, int h_y) {
    int dx = x - h_x;
    int dy = y - h_y;
    return (dx*dx + dy*dy <= 15*15);
}

unsigned int next_rand = 12345; 

// Generador de números pseudoaleatorios
unsigned int rand() {
    next_rand = next_rand * 1103515245 + 12345;
    return (unsigned int)(next_rand / 65536) % 32768;
}

// Devuelve un número aleatorio en el rango [min, max]
int rand_range(int min, int max) {
    return min + (rand() % (max - min + 1));
}

// Devuelve 1 si dos círculos se superponen
int circles_overlap(int x1, int y1, int r1, int x2, int y2, int r2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return (dx*dx + dy*dy < (r1 + r2) * (r1 + r2));
}

// Dibuja una barra horizontal de ancho completo
void drawFullWidthBar(int y, int height, uint32_t color) {
    for (int j = y; j < y + height; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            video_putPixel(i, j, color);
        }
    }
}

// Muestra un mensaje a pantalla completa
void displayFullScreenMessage(const char *message, uint32_t textColor) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            video_putPixel(x, y, COLOR_BG_BLACK);
        }
    }
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        video_putPixel(x, 10, textColor);
        video_putPixel(x, SCREEN_HEIGHT - 10, textColor);
    }
    for (int y = 10; y < SCREEN_HEIGHT - 10; y++) {
        video_putPixel(10, y, textColor);
        video_putPixel(SCREEN_WIDTH - 10, y, textColor);
    }
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    int messageLen = 0;
    const char* temp = message;
    while (*temp) { messageLen++; temp++; }
    drawTextWithBg(centerX - (messageLen * 4), centerY - 50, message, textColor, COLOR_BG_BLACK);
    for (int y = centerY - 30; y < centerY - 25; y++) {
        for (int x = centerX - (messageLen * 5); x < centerX + (messageLen * 5); x++) {
            video_putPixel(x, y, textColor);
        }
    }
}

// Dibuja la flecha del jugador
void drawPlayerArrow(int player_x, int player_y, int player_angle, int hole_x, int hole_y, int arrow_color, Player *players, int num_players) {
    int arrow_len = 18;
    int arrow_x = player_x + (cos_table[player_angle] * arrow_len) / 100;
    int arrow_y = player_y + (sin_table[player_angle] * arrow_len) / 100;
    if (arrow_y < UI_TOP_MARGIN + 5) return;
    #define IN_SCREEN(x, y) ((x) >= 0 && (x) < SCREEN_WIDTH && (y) >= UI_TOP_MARGIN && (y) < SCREEN_HEIGHT)
    int arrow_points[5][2] = {
        {arrow_x, arrow_y},
        {arrow_x + 1, arrow_y},
        {arrow_x, arrow_y + 1},
        {arrow_x - 1, arrow_y},
        {arrow_x, arrow_y - 1}
    };
    for (int i = 0; i < 5; i++) {
        int px = arrow_points[i][0];
        int py = arrow_points[i][1];
        if (IN_SCREEN(px, py)) {
            if (!isInsideHole(px, py, hole_x, hole_y)) {
                int isInsideBall = 0;
                for (int j = 0; j < num_players; j++) {
                    int dx = px - players[j].ball_x;
                    int dy = py - players[j].ball_y;
                    if (dx*dx + dy*dy <= 5*5) {
                        isInsideBall = 1;
                        break;
                    }
                }
                if (!isInsideBall) {
                    video_putPixel(px, py, arrow_color);
                }
            }
        }
    }
    #undef IN_SCREEN
}

// Borra la flecha anterior
void eraseArrow(int prev_x, int prev_y, int prev_angle, int hole_x, int hole_y, Player *players, int num_players) {
    int prev_arrow_len = 18;
    int prev_arrow_x = prev_x + (cos_table[prev_angle] * prev_arrow_len) / 100;
    int prev_arrow_y = prev_y + (sin_table[prev_angle] * prev_arrow_len) / 100;
    if (prev_arrow_y >= UI_TOP_MARGIN + 5) {
        #define IN_SCREEN(x, y) ((x) >= 0 && (x) < SCREEN_WIDTH && (y) >= UI_TOP_MARGIN && (y) < SCREEN_HEIGHT)
        int arrow_points[5][2] = {
            {prev_arrow_x, prev_arrow_y},
            {prev_arrow_x + 1, prev_arrow_y},
            {prev_arrow_x, prev_arrow_y + 1},
            {prev_arrow_x - 1, prev_arrow_y},
            {prev_arrow_x, prev_arrow_y - 1}
        };
        for (int i = 0; i < 5; i++) {
            int px = arrow_points[i][0];
            int py = arrow_points[i][1];
            if (IN_SCREEN(px, py)) {
                if (isInsideHole(px, py, hole_x, hole_y)) {
                    video_putPixel(px, py, COLOR_HOLE);
                } else {
                    int painted = 0;
                    for (int j = 0; j < num_players; j++) {
                        int dx = px - players[j].ball_x;
                        int dy = py - players[j].ball_y;
                        if (dx*dx + dy*dy <= 5*5) {
                            video_putPixel(px, py, players[j].ball_color);
                            painted = 1;
                            break;
                        }
                    }
                    if (!painted) {
                        video_putPixel(px, py, COLOR_BG_GREEN);
                    }
                }
            }
        }
        #undef IN_SCREEN
    }
}

// Dibuja texto fijo en pantalla
void drawTextFixed(int x, int y, const char *text, uint32_t color, uint32_t bg) {
    int i = 0;
    while (text[i]) {
        video_putCharXY(x + i * 8, y, text[i], color, bg);
        i++;
    }
}

void audiobounce() {
    beep(500, 100);
}