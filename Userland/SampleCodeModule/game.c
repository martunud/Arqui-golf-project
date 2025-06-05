#include "include/lib.h"
#include "include/game.h"

#define COLOR_BG_GREY 0xD3D3D3
#define COLOR_TEXT_BLUE 0x0000FF
#define COLOR_BG_GREEN 0x2ECC40
#define COLOR_PLAYER 0xFFFFFF
#define COLOR_BALL 0xFFFF00
#define COLOR_HOLE 0x000000
#define COLOR_TEXT_WHITE 0xFFFFFF
#define COLOR_BG_BLACK 0x000000 

#define SCREEN_WIDTH 1024  // Aumentar según las dimensiones reales
#define SCREEN_HEIGHT 768  // Aumentar según las dimensiones reales
#define UI_TOP_MARGIN 32

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
    for (int i = 0; text[i] != '\0'; i++) {
        video_putCharXY(x + i * 8, y, text[i], color, COLOR_BG_GREY);
    }
}

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

// Add this helper function near the top of your file:
int isInsideHole(int x, int y, int h_x, int h_y) {
    int dx = x - h_x;
    int dy = y - h_y;
    return (dx*dx + dy*dy <= 15*15); // 15 is hole radius
}

int isInsidePlayer(int x, int y, int p_x, int p_y) {
    int dx = x - p_x;
    int dy = y - p_y;
    return (dx*dx + dy*dy <= PLAYER_RADIUS * PLAYER_RADIUS);
}

unsigned int next_rand = 12345; 

unsigned int rand() {
    next_rand = next_rand * 1103515245 + 12345;
    return (unsigned int)(next_rand / 65536) % 32768;
}

int rand_range(int min, int max) {
    return min + (rand() % (max - min + 1));
}

//Chequear si sobreponen dos círculos
int circles_overlap(int x1, int y1, int r1, int x2, int y2, int r2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return (dx*dx + dy*dy < (r1 + r2) * (r1 + r2));
}


void game_main_screen() {
    video_clearScreenColor(COLOR_BG_GREY);

    const char *lines[] = {
        "Bienvenido a Pongis-Golf",
        "Presione 1 para jugar",
        "Presione ESC para salir",
        "Presione CTRL+R para tomar un snapshot"
    };
    int num_lines = 4;
    int line_height = 20; // Más compacto
    int font_width = 8;   // Asumiendo fuente monoespaciada
    int font_height = 16; // Asumiendo altura de fuente

    // Calcular alto total del bloque de texto
    int total_height = num_lines * line_height;
    int startY = (SCREEN_HEIGHT - total_height) / 2;

    for (int i = 0; i < num_lines; i++) {
        int text_len = strlen(lines[i]);
        int text_width = text_len * font_width;
        int x = (SCREEN_WIDTH - text_width) / 2;
        int y = startY + i * line_height;
        drawText(x, y, lines[i], COLOR_TEXT_BLUE);
    }

    while (1) {
        char input = getchar();
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

void drawFullWidthBar(int y, int height, uint32_t color) {
    for (int j = y; j < y + height; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            video_putPixel(i, j, color);
        }
    }
}

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
    
    drawTextWithBg(centerX - 230, centerY + 50, "Presione ENTER para seguir jugando o ESC para salir", textColor, COLOR_BG_BLACK);
}

void game_start() {
    video_clearScreenColor(COLOR_BG_GREEN);  

    int margin = 50;
    
    int player_x, player_y, ball_x, ball_y, hole_x, hole_y;
    
    hole_x = rand_range(margin + 15, SCREEN_WIDTH - margin - 15);
    hole_y = rand_range(UI_TOP_MARGIN + margin + 15, SCREEN_HEIGHT - margin - 15);
    
    do {
        player_x = rand_range(margin + PLAYER_RADIUS, SCREEN_WIDTH - margin - PLAYER_RADIUS);
        player_y = rand_range(UI_TOP_MARGIN + margin + PLAYER_RADIUS, SCREEN_HEIGHT - margin - PLAYER_RADIUS);
    } while (circles_overlap(player_x, player_y, PLAYER_RADIUS + 10, hole_x, hole_y, 15 + 10));
    
    do {
        ball_x = rand_range(margin + 5, SCREEN_WIDTH - margin - 5);
        ball_y = rand_range(UI_TOP_MARGIN + margin + 5, SCREEN_HEIGHT - margin - 5);
    } while (
        circles_overlap(ball_x, ball_y, 5 + 10, hole_x, hole_y, 15 + 10) || 
        circles_overlap(ball_x, ball_y, 5 + 10, player_x, player_y, PLAYER_RADIUS + 10)
    );
    
    int prev_ball_x = ball_x, prev_ball_y = ball_y;
    int prev_player_x = player_x, prev_player_y = player_y;
    char modo[] = "Solitario";
    int golpes = 0; // Contador de golpes
    int max_golpes = 6; // Máximo de golpes permitidos
    
    int ball_vx = 0, ball_vy = 0;
    int player_angle = 0, prev_player_angle = 0;

    int ball_in_hole = 0;
    int puede_golpear = 1;
    int dx, dy, dist2, min_dist;

    drawCircle(hole_x, hole_y, 15, COLOR_HOLE);
    drawCircle(player_x, player_y, PLAYER_RADIUS, COLOR_PLAYER);
    drawCircle(ball_x, ball_y, 5, COLOR_BALL);
    
drawFullWidthBar(0, 16, COLOR_BG_BLACK);  // Background for first text line
drawFullWidthBar(16, 16, COLOR_BG_BLACK); // Background for second text line

char status_str[40];
sprintf(status_str, "Golpes: %d/%d, Modo: %s ", golpes, max_golpes, modo);
drawTextWithBg(10, 10, status_str, COLOR_TEXT_WHITE, COLOR_BG_BLACK);
drawTextWithBg(10, 30, "<- y ->: direccion | UP: mover | ESC: salir", COLOR_TEXT_WHITE, COLOR_BG_BLACK);

    while (1) {
 
        
        if (prev_ball_x != ball_x || prev_ball_y != ball_y) {
            for (int y = -5; y <= 5; y++) {
                for (int x = -5; x <= 5; x++) {
                    if (x*x + y*y <= 5*5) {
                        int px = prev_ball_x + x;
                        int py = prev_ball_y + y;
                        
                        if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                            if (isInsidePlayer(px, py, player_x, player_y)) {
                                video_putPixel(px, py, COLOR_PLAYER);
                            }
                            else if (isInsideHole(px, py, hole_x, hole_y)) {
                                video_putPixel(px, py, COLOR_HOLE);
                            }
                            else {
                                video_putPixel(px, py, COLOR_BG_GREEN);
                            }
                        }
                    }
                }
            }
            
            drawCircle(ball_x, ball_y, 5, COLOR_BALL);
        }
        
        if (prev_player_x != player_x || prev_player_y != player_y) {
            eraseCircle(prev_player_x, prev_player_y, PLAYER_RADIUS);
            drawCircle(player_x, player_y, PLAYER_RADIUS, COLOR_PLAYER);
        }

        if (prev_player_angle != player_angle || prev_player_x != player_x || prev_player_y != player_y) {
            int prev_arrow_len = 18;
            int prev_arrow_x = prev_player_x + (cos_table[prev_player_angle] * prev_arrow_len) / 100;
            int prev_arrow_y = prev_player_y + (sin_table[prev_player_angle] * prev_arrow_len) / 100;
            
            // Solo borrar si la flecha anterior no estaba en la región de texto
            if (prev_arrow_y >= UI_TOP_MARGIN + 5) {
                if (isInsideHole(prev_arrow_x, prev_arrow_y, hole_x, hole_y))
                    video_putPixel(prev_arrow_x, prev_arrow_y, COLOR_HOLE);
                else
                    video_putPixel(prev_arrow_x, prev_arrow_y, COLOR_BG_GREEN);
                    
                if (isInsideHole(prev_arrow_x + 1, prev_arrow_y, hole_x, hole_y))
                    video_putPixel(prev_arrow_x + 1, prev_arrow_y, COLOR_HOLE);
                else
                    video_putPixel(prev_arrow_x + 1, prev_arrow_y, COLOR_BG_GREEN);
                    
                if (isInsideHole(prev_arrow_x, prev_arrow_y + 1, hole_x, hole_y))
                    video_putPixel(prev_arrow_x, prev_arrow_y + 1, COLOR_HOLE);
                else
                    video_putPixel(prev_arrow_x, prev_arrow_y + 1, COLOR_BG_GREEN);
                    
                if (isInsideHole(prev_arrow_x - 1, prev_arrow_y, hole_x, hole_y))
                    video_putPixel(prev_arrow_x - 1, prev_arrow_y, COLOR_HOLE);
                else
                    video_putPixel(prev_arrow_x - 1, prev_arrow_y, COLOR_BG_GREEN);
                    
                if (isInsideHole(prev_arrow_x, prev_arrow_y - 1, hole_x, hole_y))
                    video_putPixel(prev_arrow_x, prev_arrow_y - 1, COLOR_HOLE);
                else
                    video_putPixel(prev_arrow_x, prev_arrow_y - 1, COLOR_BG_GREEN);
            }
            
            // Dibujar la nueva flecha (solo si la pelota está quieta y no en el hoyo)
            if (ball_vx == 0 && ball_vy == 0 && !ball_in_hole) {
                drawPlayerArrow(player_x, player_y, player_angle, hole_x, hole_y);
            }
        }
        
        prev_ball_x = ball_x;
        prev_ball_y = ball_y;
        prev_player_x = player_x;
        prev_player_y = player_y;  
        prev_player_angle = player_angle;

        // ---- INPUT NO BLOQUEANTE ----
        char input = 0;
        if (try_getchar(&input)) {
            if (input == 27) { // ESC
                clearScreen();
                break;
            } else if ((unsigned char)input == 0x12) { // CTRL+R (código 0x12)
                takeRegistersSnapshot();
                // No mostrar ningún mensaje - simplemente tomar el snapshot
            }

            // SOLO permitir controles de jugador si la pelota está quieta y no está en el hoyo
            if (!ball_in_hole) {  
                if (input == (char)0x80) { 
                    int old_player_x = player_x;
                    int old_player_y = player_y;
                    
                    player_x += (cos_table[player_angle] * 10) / 100;
                    player_y += (sin_table[player_angle] * 10) / 100;

                    // Evitar que salga de pantalla
                    if (player_x < PLAYER_RADIUS) player_x = PLAYER_RADIUS;
                    if (player_x >= SCREEN_WIDTH - PLAYER_RADIUS) player_x = SCREEN_WIDTH - PLAYER_RADIUS - 1;
                    if (player_y < PLAYER_RADIUS + UI_TOP_MARGIN) player_y = PLAYER_RADIUS + UI_TOP_MARGIN;
                    if (player_y >= SCREEN_HEIGHT - PLAYER_RADIUS) player_y = SCREEN_HEIGHT - PLAYER_RADIUS - 1;
                    
                    int hole_dx = player_x - hole_x;
                    int hole_dy = player_y - hole_y;
                    int hole_dist2 = hole_dx * hole_dx + hole_dy * hole_dy;
                    int min_collision_dist = PLAYER_RADIUS + 15; 
                    
                    if (hole_dist2 < min_collision_dist * min_collision_dist) {
                        player_x = old_player_x;
                        player_y = old_player_y;
                    }
                    
                    dx = ball_x - player_x;
                    dy = ball_y - player_y;
                    dist2 = dx*dx + dy*dy;
                    min_dist = PLAYER_RADIUS + 5;
                    if (dist2 <= min_dist * min_dist && puede_golpear) {
                        int golpe_power = 30;                         
                        ball_vx = (cos_table[player_angle] * golpe_power) / 10;
                        ball_vy = (sin_table[player_angle] * golpe_power) / 10;
                        puede_golpear = 0;
                        
                        // Incrementar contador de golpes
                        golpes++;
                        
                        // Verificar si se alcanzó el límite de golpes
                        if (golpes >= max_golpes && !ball_in_hole) {
                            char game_over_msg[80];
                            sprintf(game_over_msg, "GAME OVER! Demasiados golpes. Presione ESPACIO para reiniciar o ESC para salir.");
                            displayFullScreenMessage(game_over_msg, COLOR_TEXT_BLUE);
                            
                            while (1) {
                                char input = 0;
                                if (try_getchar(&input)) {
                                    if (input == 27) { // ESC
                                        clearScreen();
                                        return;
                                    } else if (input == ' ') { // Barra espaciadora
                                        // Reiniciar el juego
                                        clearScreen();
                                        game_start();
                                        return;
                                    }
                                }
                                for (volatile int i = 0; i < 100000; i++);
                            }
                        }
                    }
                } else if (input == (char)0x82) { // Flecha izquierda
                    player_angle = (player_angle + 35) % 36;
                } else if (input == (char)0x83) { // Flecha derecha
                    player_angle = (player_angle + 1) % 36;
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
        if (ball_y < UI_TOP_MARGIN + 5) { ball_y = UI_TOP_MARGIN + 5; ball_vy = -ball_vy; }
        if (ball_y > SCREEN_HEIGHT - 5) { ball_y = SCREEN_HEIGHT - 5; ball_vy = -ball_vy; }

        // Verificar si la pelota llegó al hoyo
        int hx = ball_x - hole_x;
        int hy = ball_y - hole_y;
        int hole_dist2 = hx * hx + hy * hy;
        if (hole_dist2 <= 100 && !ball_in_hole) {  
            drawFullWidthBar(0, 16, COLOR_BG_BLACK);
            drawTextWithBg(10, 10, "PELOTA EN HOYO!", COLOR_TEXT_WHITE, COLOR_BG_BLACK);
            sleep(500); 
            
            ball_in_hole = 1;
            ball_vx = 0;
            ball_vy = 0;
            
            // Mover la pelota exactamente al centro del hoyo para mejor visualización
            ball_x = hole_x;
            ball_y = hole_y;
            
            char victory_msg[80];
            sprintf(victory_msg, "VICTORIA! Hoyo completado en %d golpes. Presione ESPACIO para reiniciar o ESC para salir.", golpes);
            displayFullScreenMessage(victory_msg, COLOR_TEXT_BLUE);
            
            while (1) {
                char input = 0;
                if (try_getchar(&input)) {
                    if (input == 27) { // ESC
                        clearScreen();
                        return;
                    } else if (input == ' ' || input == '\n' || input == '\r') { // Espaciadora, Enter
                        // Reiniciar el juego
                        clearScreen();
                        game_start();
                        return;
                    }
                }
                for (volatile int i = 0; i < 100000; i++);
            }
        }

        sleep(16);  // ~60 FPS (16.7ms per frame)
    }
}

// Actualiza la función que dibuja la flecha
void drawPlayerArrow(int player_x, int player_y, int player_angle, int hole_x, int hole_y) {
    int arrow_len = 18;
    int arrow_x = player_x + (cos_table[player_angle] * arrow_len) / 100;
    int arrow_y = player_y + (sin_table[player_angle] * arrow_len) / 100;
    
    // No dibujar la flecha si está en la región de texto UI (parte superior de la pantalla)
    if (arrow_y < UI_TOP_MARGIN + 5) {
        return;
    }
    
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
                video_putPixel(px, py, COLOR_TEXT_BLUE);
            }
        }
    }
    #undef IN_SCREEN
}
