#include "include/lib.h"
#include "include/game.h"

#define COLOR_BG_GREY 0xD3D3D3
#define COLOR_TEXT_BLUE 0x0000FF
#define COLOR_BG_GREEN 0x2ECC40
#define COLOR_PLAYER 0xFFFFFF
#define COLOR_BALL 0xFFFF00
#define COLOR_HOLE 0x000000
#define COLOR_TEXT_WHITE 0xFFFFFF

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_RADIUS 10

// Tabla de seno y coseno para 36 ángulos (cada 10 grados)
const int cos_table[36] = {100,98,94,87,77,64,50,34,17,0,-17,-34,-50,-64,-77,-87,-94,-98,-100,-98,-94,-87,-77,-64,-50,-34,-17,0,17,34,50,64,77,87,94,98};
const int sin_table[36] = {0,17,34,50,64,77,87,94,98,100,98,94,87,77,64,50,34,17,0,-17,-34,-50,-64,-77,-87,-94,-98,-100,-98,-94,-87,-77,-64,-50,-34,-17};

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
    int lines = y / 16;
    int spaces = x / 8;
    for (int i = 0; i < lines; i++) video_putChar('\n', color, COLOR_BG_GREY);
    for (int i = 0; i < spaces; i++) video_putChar(' ', color, COLOR_BG_GREY);
    while (*text) {
        video_putChar(*text, color, COLOR_BG_GREY);
        text++;
    }
}

void game_main_screen() {
    video_clearScreenColor(COLOR_BG_GREY);
    int centerX = 400, centerY = 300;

    drawText(centerX - 120, centerY - 50, "Bienvenido a Pongis-Golf", COLOR_TEXT_BLUE);
    drawText(centerX - 100, centerY, "Presione 1 para empezar", COLOR_TEXT_BLUE);
    drawText(centerX - 80, centerY + 50, "Presione ESC para salir", COLOR_TEXT_BLUE);

    while (1) {
        char input = getchar();
        printf("Input: %d\n", input); // Debugging line to see input values
        if (input != 0) {
            if (input == '1') {
                game_start();
                break;
            } else if (input == 27) {
                clearScreen();
                return;
            }
        }
        for (volatile int i = 0; i < 100000; i++); // delay
    }
}

void game_start() {
    video_clearScreenColor(COLOR_BG_GREEN);

    int ball_x = 100, ball_y = 300;
    int hole_x = 700, hole_y = 300;
    int power = 0;
    int player_x = 50, player_y = 300;

    int ball_vx = 0, ball_vy = 0;
    int player_angle = 0;

    int ball_in_hole = 0;
    int puede_golpear = 1;
    int dx, dy, dist2, min_dist;

    while (1) {
        video_clearScreenColor(COLOR_BG_GREEN);

        drawCircle(hole_x, hole_y, 15, COLOR_HOLE);
        drawCircle(player_x, player_y, PLAYER_RADIUS, COLOR_PLAYER);

        if (!ball_in_hole) {
            drawCircle(ball_x, ball_y, 5, COLOR_BALL);
        }

        int arrow_len = 18;
        int arrow_x = player_x + (cos_table[player_angle] * arrow_len) / 100;
        int arrow_y = player_y + (sin_table[player_angle] * arrow_len) / 100;
        video_putPixel(arrow_x, arrow_y, COLOR_TEXT_BLUE);
        video_putPixel(arrow_x + 1, arrow_y, COLOR_TEXT_BLUE);
        video_putPixel(arrow_x, arrow_y + 1, COLOR_TEXT_BLUE);
        video_putPixel(arrow_x - 1, arrow_y, COLOR_TEXT_BLUE);
        video_putPixel(arrow_x, arrow_y - 1, COLOR_TEXT_BLUE);

        char power_str[40];
        sprintf(power_str, "Power: %d  Angulo: %d", power, player_angle * 10);
        drawText(10, 10, power_str, COLOR_TEXT_WHITE);
        drawText(10, 30, "W/S: power | <- y ->: direccion | UP: mover | ESC: salir", COLOR_TEXT_WHITE);

        char input = getchar();
        if (input != 0) {
            if (input == 27) {
                clearScreen();
                break;
            } else if (input == 'w' || input == 'W') {
                if (power < 100) power += 5;
            } else if (input == 's' || input == 'S') {
                if (power > 0) power -= 5;
            } else if (input == (char)0x80) {
                player_x += (cos_table[player_angle] * 10) / 100;
                player_y += (sin_table[player_angle] * 10) / 100;

                dx = ball_x - player_x;
                dy = ball_y - player_y;
                dist2 = dx*dx + dy*dy;
                min_dist = PLAYER_RADIUS + 5;
                if (dist2 <= min_dist * min_dist && ball_vx == 0 && ball_vy == 0 && !ball_in_hole && puede_golpear) {
                    int golpe_power = (power > 0) ? power : 20;
                    ball_vx = (cos_table[player_angle] * golpe_power) / 10;
                    ball_vy = (sin_table[player_angle] * golpe_power) / 10;
                    puede_golpear = 0;
                }
            } else if (input == (char)0x82) {
                player_angle = (player_angle + 35) % 36;
            } else if (input == (char)0x83) {
                player_angle = (player_angle + 1) % 36;
            }
        }

        if (player_x < PLAYER_RADIUS) player_x = PLAYER_RADIUS;
        if (player_x > SCREEN_WIDTH - PLAYER_RADIUS) player_x = SCREEN_WIDTH - PLAYER_RADIUS;
        if (player_y < PLAYER_RADIUS) player_y = PLAYER_RADIUS;
        if (player_y > SCREEN_HEIGHT - PLAYER_RADIUS) player_y = SCREEN_HEIGHT - PLAYER_RADIUS;

        dx = ball_x - player_x;
        dy = ball_y - player_y;
        dist2 = dx*dx + dy*dy;
        min_dist = PLAYER_RADIUS + 5;
        if (dist2 > min_dist * min_dist) {
            puede_golpear = 1;
        }

        if ((ball_vx != 0 || ball_vy != 0) && !ball_in_hole) {
            ball_x += ball_vx / 10;
            ball_y += ball_vy / 10;
            ball_vx = (ball_vx * 95) / 100;
            ball_vy = (ball_vy * 95) / 100;
            if (ball_vx < 1 && ball_vx > -1) ball_vx = 0;
            if (ball_vy < 1 && ball_vy > -1) ball_vy = 0;
        }

        if (ball_x < 5) ball_x = 5;
        if (ball_x > SCREEN_WIDTH - 5) ball_x = SCREEN_WIDTH - 5;
        if (ball_y < 5) ball_y = 5;
        if (ball_y > SCREEN_HEIGHT - 5) ball_y = SCREEN_HEIGHT - 5;

        int hx = ball_x - hole_x;
        int hy = ball_y - hole_y;
        int hole_dist2 = hx * hx + hy * hy;
        if (hole_dist2 <= 100 && !ball_in_hole) {
            ball_in_hole = 1;
            ball_vx = 0;
            ball_vy = 0;
            drawText(300, 100, "¡Hoyo! Presione ESC para salir", COLOR_TEXT_BLUE);
            while (1) {
                char esc_input = getchar();
                if (esc_input == 27) {
                    clearScreen();
                    return;
                }
                for (volatile int i = 0; i < 100000; i++);
            }
        }

        for (volatile int i = 0; i < 100000; i++); // delay del bucle principal
    }
}
