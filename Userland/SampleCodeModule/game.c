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
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    video_putPixel(px, py, color);
                }
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

void eraseCircle(int cx, int cy, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                video_putPixel(cx + x, cy + y, COLOR_BG_GREEN);
            }
        }
    }
}

void eraseTextArea(int x, int y, int width, int height) {
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            video_putPixel(i, j, COLOR_BG_GREEN);
        }
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
    video_clearScreenColor(COLOR_BG_GREEN);  // Initial full clear

    int ball_x = 100, ball_y = 300;
    int prev_ball_x = ball_x, prev_ball_y = ball_y;
    int hole_x = 700, hole_y = 300;
    int power = 0, prev_power = 0;
    int player_x = 50, player_y = 300;
    int prev_player_x = player_x, prev_player_y = player_y;

    int ball_vx = 0, ball_vy = 0;
    int player_angle = 0, prev_player_angle = 0;

    int ball_in_hole = 0;
    int puede_golpear = 1;
    int dx, dy, dist2, min_dist;
    int needs_redraw = 1; // Start with initial draw
    
    // Initial draw of all elements
    drawCircle(hole_x, hole_y, 15, COLOR_HOLE);
    drawCircle(player_x, player_y, PLAYER_RADIUS, COLOR_PLAYER);
    drawCircle(ball_x, ball_y, 5, COLOR_BALL);
    
    char power_str[40];
    sprintf(power_str, "Power: %d  Angulo: %d", power, player_angle * 10);
    drawText(10, 10, power_str, COLOR_TEXT_WHITE);
    drawText(10, 30, "W/S: power | <- y ->: direccion | UP: mover | ESC: salir", COLOR_TEXT_WHITE);

    while (1) {
        // Don't clear the entire screen
        // Instead, selectively erase and redraw only what changed
        
        if (prev_ball_x != ball_x || prev_ball_y != ball_y) {
            // Ball moved - erase old position and draw new one
            eraseCircle(prev_ball_x, prev_ball_y, 5);
            drawCircle(ball_x, ball_y, 5, COLOR_BALL);
        }
        
        if (prev_player_x != player_x || prev_player_y != player_y) {
            // Player moved - erase old position and draw new one
            eraseCircle(prev_player_x, prev_player_y, PLAYER_RADIUS);
            drawCircle(player_x, player_y, PLAYER_RADIUS, COLOR_PLAYER);
        }

            int arrow_len = 18;
        int arrow_x = player_x + (cos_table[player_angle] * arrow_len) / 100;
        int arrow_y = player_y + (sin_table[player_angle] * arrow_len) / 100;

        // Solo mostrar flecha si la pelota está quieta y no está en el hoyo
        if (ball_vx == 0 && ball_vy == 0 && !ball_in_hole) {
            // Helper para evitar dibujar fuera de pantalla
            #define IN_SCREEN(x, y) ((x) >= 0 && (x) < SCREEN_WIDTH && (y) >= 0 && (y) < SCREEN_HEIGHT)
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
                if (IN_SCREEN(px, py))
                    video_putPixel(px, py, COLOR_TEXT_BLUE);
            }
            #undef IN_SCREEN
        }
        
        // Handle direction indicator (blue arrow)
        if (prev_player_angle != player_angle || prev_player_x != player_x || prev_player_y != player_y) {
            // Erase old arrow
            int prev_arrow_len = 18;
            int prev_arrow_x = prev_player_x + (cos_table[prev_player_angle] * prev_arrow_len) / 100;
            int prev_arrow_y = prev_player_y + (sin_table[prev_player_angle] * prev_arrow_len) / 100;
            
            video_putPixel(prev_arrow_x, prev_arrow_y, COLOR_BG_GREEN);
            video_putPixel(prev_arrow_x + 1, prev_arrow_y, COLOR_BG_GREEN);
            video_putPixel(prev_arrow_x, prev_arrow_y + 1, COLOR_BG_GREEN);
            video_putPixel(prev_arrow_x - 1, prev_arrow_y, COLOR_BG_GREEN);
            video_putPixel(prev_arrow_x, prev_arrow_y - 1, COLOR_BG_GREEN);
            
            // Draw new arrow
            int arrow_len = 18;
            int arrow_x = player_x + (cos_table[player_angle] * arrow_len) / 100;
            int arrow_y = player_y + (sin_table[player_angle] * arrow_len) / 100;
            
            if (ball_vx == 0 && ball_vy == 0 && !ball_in_hole) {
                // Helper para evitar dibujar fuera de pantalla
                #define IN_SCREEN(x, y) ((x) >= 0 && (x) < SCREEN_WIDTH && (y) >= 0 && (y) < SCREEN_HEIGHT)
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
                    if (IN_SCREEN(px, py))
                        video_putPixel(px, py, COLOR_TEXT_BLUE);
                }
                #undef IN_SCREEN
            }
        }
        
        // Update power text only if changed
        if (prev_power != power) {
            eraseTextArea(10, 10, 300, 16);  // Erase old text area
            sprintf(power_str, "Power: %d  Angulo: %d", power, player_angle * 10);
            drawText(10, 10, power_str, COLOR_TEXT_WHITE);
        }
        
        // Save current state for next comparison
        prev_ball_x = ball_x;
        prev_ball_y = ball_y;
        prev_player_x = player_x;
        prev_player_y = player_y;
        prev_player_angle = player_angle;
        prev_power = power;

        // ---- INPUT NO BLOQUEANTE ----
        char input = 0;
        if (try_getchar(&input)) {
            if (input == 27) { // ESC
                clearScreen();
                break;
            }

            // SOLO permitir controles de jugador si la pelota está quieta y no está en el hoyo
            if (ball_vx == 0 && ball_vy == 0 && !ball_in_hole) {
                if (input == 'w' || input == 'W') {
                    if (power < 100) power += 5;
                    needs_redraw = 1;
                } else if (input == 's' || input == 'S') {
                    if (power > 0) power -= 5;
                    needs_redraw = 1;
                } else if (input == (char)0x80) { // Flecha arriba: mover jugador
                    player_x += (cos_table[player_angle] * 10) / 100;
                    player_y += (sin_table[player_angle] * 10) / 100;

                    // Evitar que salga de pantalla
                    if (player_x < PLAYER_RADIUS) player_x = PLAYER_RADIUS;
                    if (player_x > SCREEN_WIDTH - PLAYER_RADIUS) player_x = SCREEN_WIDTH - PLAYER_RADIUS;
                    if (player_y < PLAYER_RADIUS) player_y = PLAYER_RADIUS;
                    if (player_y > SCREEN_HEIGHT - PLAYER_RADIUS) player_y = SCREEN_HEIGHT - PLAYER_RADIUS;

                    // Si después de mover está cerca de la pelota, puede golpear
                    dx = ball_x - player_x;
                    dy = ball_y - player_y;
                    dist2 = dx*dx + dy*dy;
                    min_dist = PLAYER_RADIUS + 5;
                    if (dist2 <= min_dist * min_dist && puede_golpear) {
                        int golpe_power = (power > 0) ? power : 20;
                        ball_vx = (cos_table[player_angle] * golpe_power) / 10;
                        ball_vy = (sin_table[player_angle] * golpe_power) / 10;
                        puede_golpear = 0;
                    }
                } else if (input == (char)0x82) { // Flecha izquierda
                    player_angle = (player_angle + 35) % 36;
                    needs_redraw = 1;
                } else if (input == (char)0x83) { // Flecha derecha
                    player_angle = (player_angle + 1) % 36;
                    needs_redraw = 1;
                }
            }
        }

        // Permitir volver a golpear solo si el jugador está lejos de la pelota
        dx = ball_x - player_x;
        dy = ball_y - player_y;
        dist2 = dx*dx + dy*dy;
        min_dist = PLAYER_RADIUS + 5;
        if (dist2 > min_dist * min_dist) {
            puede_golpear = 1;
        }

        // ---- FÍSICA DE LA PELOTA ----
        if ((ball_vx != 0 || ball_vy != 0) && !ball_in_hole) {
            ball_x += ball_vx / 10;
            ball_y += ball_vy / 10;
            ball_vx = (ball_vx * 95) / 100;
            ball_vy = (ball_vy * 95) / 100;
            if (ball_vx < 1 && ball_vx > -1) ball_vx = 0;
            if (ball_vy < 1 && ball_vy > -1) ball_vy = 0;
        }

        // Rebote simple en bordes
        if (ball_x < 5) { ball_x = 5; ball_vx = -ball_vx; }
        if (ball_x > SCREEN_WIDTH - 5) { ball_x = SCREEN_WIDTH - 5; ball_vx = -ball_vx; }
        if (ball_y < 5) { ball_y = 5; ball_vy = -ball_vy; }
        if (ball_y > SCREEN_HEIGHT - 5) { ball_y = SCREEN_HEIGHT - 5; ball_vy = -ball_vy; }

        // Verificar si la pelota llegó al hoyo
        int hx = ball_x - hole_x;
        int hy = ball_y - hole_y;
        int hole_dist2 = hx * hx + hy * hy;
        if (hole_dist2 <= 100 && !ball_in_hole) {
            ball_in_hole = 1;
            ball_vx = 0;
            ball_vy = 0;
            drawText(300, 100, "¡Hoyo! Presione ESC para salir", COLOR_TEXT_BLUE);
            while (1) {
                char esc_input = 0;
                if (try_getchar(&esc_input) && esc_input == 27) {
                    clearScreen();
                    return;
                }
                for (volatile int i = 0; i < 100000; i++);
            }
        }

        // Optimize your sleep time for smoother animation
        sleep(16);  // ~60 FPS (16.7ms per frame)
    }
}
