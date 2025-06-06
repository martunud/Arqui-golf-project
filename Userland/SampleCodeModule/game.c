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

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define UI_TOP_MARGIN 32
#define PLAYER_RADIUS 10

const int cos_table[36] = {100,98,94,87,77,64,50,34,17,0,-17,-34,-50,-64,-77,-87,-94,-98,-100,-98,-94,-87,-77,-64,-50,-34,-17,0,17,34,50,64,77,87,94,98};
const int sin_table[36] = {0,17,34,50,64,77,87,94,98,100,98,94,87,77,64,50,34,17,0,-17,-34,-50,-64,-77,-87,-94,-98,-100,-98,-94,-87,-77,-64,-50,-34,-17};

typedef struct {
    int x, y, prev_x, prev_y;
    int angle, prev_angle;
    int golpes;
    int puede_golpear;
    int color;
    int arrow_color;
    int ball_x, ball_y, prev_ball_x, prev_ball_y;
    int ball_vx, ball_vy;
    int ball_in_hole;
    int ball_color;
    // Controles
    char control_up, control_left, control_right;
    char *name;
} Player;

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

void eraseBallSmart(int prev_ball_x, int prev_ball_y, Player *players, int num_players, int hole_x, int hole_y) {
    for (int y = -5; y <= 5; y++) {
        for (int x = -5; x <= 5; x++) {
            if (x*x + y*y <= 5*5) {
                int px = prev_ball_x + x;
                int py = prev_ball_y + y;
                if (px >= 0 && px < SCREEN_WIDTH && py >= UI_TOP_MARGIN && py < SCREEN_HEIGHT) {
                    int painted = 0;
                    
                    // Si hay un jugador en ese pixel, repintarlo
                    for (int j = 0; j < num_players; j++) {
                        if ((px - players[j].x)*(px - players[j].x) + (py - players[j].y)*(py - players[j].y) <= PLAYER_RADIUS*PLAYER_RADIUS) {
                            video_putPixel(px, py, players[j].color);
                            painted = 1;
                            break;
                        }
                    }
                    
                    // Si hay un hoyo, repintar el hoyo
                    if (!painted && ((px - hole_x)*(px - hole_x) + (py - hole_y)*(py - hole_y) <= 15*15)) {
                        video_putPixel(px, py, COLOR_HOLE);
                        painted = 1;
                    }
                    
                    // Si no hay nada más ahí, pintar el fondo
                    if (!painted) {
                        video_putPixel(px, py, COLOR_BG_GREEN);
                    }
                }
            }
        }
    }
}

void erasePlayerSmart(int prev_x, int prev_y, Player *players, int num_players, int hole_x, int hole_y) {
    for (int y = -PLAYER_RADIUS; y <= PLAYER_RADIUS; y++) {
        for (int x = -PLAYER_RADIUS; x <= PLAYER_RADIUS; x++) {
            if (x*x + y*y <= PLAYER_RADIUS*PLAYER_RADIUS) {
                int px = prev_x + x;
                int py = prev_y + y;
                if (px >= 0 && px < SCREEN_WIDTH && py >= UI_TOP_MARGIN && py < SCREEN_HEIGHT) {
                    int painted = 0;
                    
                    // Si hay una pelota en ese pixel, la repinto
                    for (int j = 0; j < num_players; j++) {
                        if ((px - players[j].ball_x)*(px - players[j].ball_x) + (py - players[j].ball_y)*(py - players[j].ball_y) <= 5*5) {
                            video_putPixel(px, py, players[j].ball_color);
                            painted = 1;
                            break;
                        }
                    }
                    
                    // Si hay hoyo, repinto hoyo
                    if (!painted && ((px - hole_x)*(px - hole_x) + (py - hole_y)*(py - hole_y) <= 15*15)) {
                        video_putPixel(px, py, COLOR_HOLE);
                        painted = 1;
                    }
                    
                    // Si no, fondo
                    if (!painted) {
                        video_putPixel(px, py, COLOR_BG_GREEN);
                    }
                }
            }
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
}

// Dibuja la flecha del jugador con el color específico
void drawPlayerArrow(int player_x, int player_y, int player_angle, int hole_x, int hole_y, int arrow_color) {
    int arrow_len = 18;
    int arrow_x = player_x + (cos_table[player_angle] * arrow_len) / 100;
    int arrow_y = player_y + (sin_table[player_angle] * arrow_len) / 100;
    
    // No dibujar la flecha si está en la región de texto UI (parte superior de la pantalla)
    if (arrow_y < UI_TOP_MARGIN + 5) {
        return;
    }
    
    #define IN_SCREEN(x, y) ((x) >= 0 && (x) < SCREEN_WIDTH && (y) >= UI_TOP_MARGIN && (y) < SCREEN_HEIGHT)
    
    // Puntos de la flecha (forma de cruz)
    int arrow_points[5][2] = {
        {arrow_x, arrow_y},         // Centro
        {arrow_x + 1, arrow_y},     // Derecha
        {arrow_x, arrow_y + 1},     // Abajo
        {arrow_x - 1, arrow_y},     // Izquierda
        {arrow_x, arrow_y - 1}      // Arriba
    };
    
    // Dibujar cada punto de la flecha
    for (int i = 0; i < 5; i++) {
        int px = arrow_points[i][0];
        int py = arrow_points[i][1];
        if (IN_SCREEN(px, py)) {
            // Solo dibujar si no está dentro del hoyo
            if (!isInsideHole(px, py, hole_x, hole_y)) { 
                video_putPixel(px, py, arrow_color);
            }
        }
    }
    #undef IN_SCREEN
}

void eraseArrow(int prev_x, int prev_y, int prev_angle, int hole_x, int hole_y) {
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
                // Si está dentro del hoyo, pintarlo como hoyo
                if (isInsideHole(px, py, hole_x, hole_y)) {
                    video_putPixel(px, py, COLOR_HOLE);
                } else {
                    // Si no, pintar como fondo
                    video_putPixel(px, py, COLOR_BG_GREEN);
                }
            }
        }
        #undef IN_SCREEN
    }
}

void drawTextFixed(int x, int y, const char *text, uint32_t color, uint32_t bg) {
    int i = 0;
    while (text[i]) {
        video_putCharXY(x + i * 8, y, text[i], color, bg);
        i++;
    }
}

void game_main_screen() {
    video_clearScreenColor(COLOR_BG_GREY);

    const char *lines[] = {
        "Bienvenido a Pongis-Golf",
        "Presione 1 para jugar",
        "Presione 2 para dos jugadores",
        "Presione ESC para salir",
        "Presione CTRL+R para tomar un snapshot"
    };
    int num_lines = 5;
    int line_height = 20;
    int font_width = 8;
    int font_height = 16;

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
                game_start(1);
                break;
            } else if (input == '2') {
                game_start(2);
                break;
            } else if (input == 27) {
                clearScreen();
                return;
            }
        }
        for (volatile int i = 0; i < 100000; i++);
    }
}

void game_start(int num_players) {
    video_clearScreenColor(COLOR_BG_GREEN);

    int margin = 50;
    int hole_x = rand_range(margin + 15, SCREEN_WIDTH - margin - 15);
    int hole_y = rand_range(UI_TOP_MARGIN + margin + 15, SCREEN_HEIGHT - margin - 15);

    Player players[2] = {0};

    // --- Configuración de jugadores ---
    // Jugador 1 (blanco)
    players[0].color = COLOR_PLAYER;         // Color del jugador
    players[0].arrow_color = COLOR_TEXT_BLUE; // Color de la flecha (azul)
    players[0].ball_color = COLOR_BALL;      // Color de la pelota (amarillo)
    players[0].control_up = (char)0x80;      // Flecha arriba
    players[0].control_left = (char)0x82;    // Flecha izquierda
    players[0].control_right = (char)0x83;   // Flecha derecha
    players[0].name = "Blanco";

    // Jugador 2 (azul) - Solo se configura si hay dos jugadores
    if (num_players == 2) {
        players[1].color = 0x0000FF;         // Color del jugador (azul)
        players[1].arrow_color = 0xFFA500;   // Color de la flecha (naranja)
        players[1].ball_color = 0xFFA500;    // Color de la pelota (naranja)
        players[1].control_up = 'w';         // W
        players[1].control_left = 'a';       // A
        players[1].control_right = 'd';      // D
        players[1].name = "Azul";
    }

    // --- Inicialización de posiciones ---
    for (int i = 0; i < num_players; i++) {
        do {
            players[i].x = rand_range(margin + PLAYER_RADIUS, SCREEN_WIDTH - margin - PLAYER_RADIUS);
            players[i].y = rand_range(UI_TOP_MARGIN + margin + PLAYER_RADIUS, SCREEN_HEIGHT - margin - PLAYER_RADIUS);
        } while (circles_overlap(players[i].x, players[i].y, PLAYER_RADIUS + 10, hole_x, hole_y, 15 + 10) ||
                 (i == 1 && circles_overlap(players[0].x, players[0].y, PLAYER_RADIUS * 2 + 10, players[1].x, players[1].y, PLAYER_RADIUS + 10)));

        do {
            players[i].ball_x = rand_range(margin + 5, SCREEN_WIDTH - margin - 5);
            players[i].ball_y = rand_range(UI_TOP_MARGIN + margin + 5, SCREEN_HEIGHT - margin - 5);
        } while (
            circles_overlap(players[i].ball_x, players[i].ball_y, 5 + 10, hole_x, hole_y, 15 + 10) ||
            circles_overlap(players[i].ball_x, players[i].ball_y, 5 + 10, players[i].x, players[i].y, PLAYER_RADIUS + 10) ||
            (i == 1 && circles_overlap(players[0].ball_x, players[0].ball_y, 5 + 10, players[1].ball_x, players[1].ball_y, 5 + 10))
        );

        players[i].prev_x = players[i].x;
        players[i].prev_y = players[i].y;
        players[i].prev_angle = players[i].angle = 0;
        players[i].prev_ball_x = players[i].ball_x;
        players[i].prev_ball_y = players[i].ball_y;
        players[i].golpes = 0;
        players[i].puede_golpear = 1;
        players[i].ball_vx = 0;
        players[i].ball_vy = 0;
        players[i].ball_in_hole = 0;
    }

    // --- Dibujo inicial ---
    drawCircle(hole_x, hole_y, 15, COLOR_HOLE);
    for (int i = 0; i < num_players; i++) {
        drawCircle(players[i].x, players[i].y, PLAYER_RADIUS, players[i].color);
        drawCircle(players[i].ball_x, players[i].ball_y, 5, players[i].ball_color);
        drawPlayerArrow(players[i].x, players[i].y, players[i].angle, hole_x, hole_y, players[i].arrow_color);
    }

    drawFullWidthBar(0, 16, COLOR_BG_BLACK);
    drawFullWidthBar(16, 16, COLOR_BG_BLACK);

    int last_golpes_p1 = -1, last_golpes_p2 = -1;
    char status_str[80];

    if (num_players == 1) {
        sprintf(status_str, "Golpes: %d, Modo: Solitario", players[0].golpes);
        drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BG_BLACK);
        last_golpes_p1 = players[0].golpes;
    } else {
        sprintf(status_str, "P1: %d | P2: %d   ESC: salir", players[0].golpes, players[1].golpes);
        drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BG_BLACK);
        last_golpes_p1 = players[0].golpes;
        last_golpes_p2 = players[1].golpes;
    }

    int ganador = -1;
    while (1) {
        // --- Borrar elementos en orden inverso al dibujo ---
        for (int i = 0; i < num_players; i++) {
            // 1. Primero borrar flechas (están encima)
            eraseArrow(players[i].prev_x, players[i].prev_y, players[i].prev_angle, hole_x, hole_y);
            
            // 2. Borrar pelotas
            eraseBallSmart(players[i].prev_ball_x, players[i].prev_ball_y, players, num_players, hole_x, hole_y);
            
            // 3. Borrar jugadores
            erasePlayerSmart(players[i].prev_x, players[i].prev_y, players, num_players, hole_x, hole_y);
        }
        // --- Actualizar barra superior si es necesario ---
        if (num_players == 1) {
            if (players[0].golpes != last_golpes_p1) {
                drawFullWidthBar(0, 16, COLOR_BG_BLACK);
                sprintf(status_str, "Golpes: %d, Modo: Solitario", players[0].golpes);
                drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BG_BLACK);
                last_golpes_p1 = players[0].golpes;
            }
        } else {
            if (players[0].golpes != last_golpes_p1 || players[1].golpes != last_golpes_p2) {
                drawFullWidthBar(0, 16, COLOR_BG_BLACK);
                sprintf(status_str, "P1: %d | P2: %d   ESC: salir", players[0].golpes, players[1].golpes);
                drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BG_BLACK);
                last_golpes_p1 = players[0].golpes;
                last_golpes_p2 = players[1].golpes;
            }
        }

        // --- INPUT NO BLOQUEANTE ---
        char input = 0;
        if (try_getchar(&input)) {
            if (input == 27) { clearScreen(); break; }
            if ((unsigned char)input == 0x12) { takeRegistersSnapshot(); }

            for (int i = 0; i < num_players; i++) {
                if (!players[i].ball_in_hole) {
                    char up = players[i].control_up, left = players[i].control_left, right = players[i].control_right;
                    if ((i == 1 && (input == up || input == up - 32)) || (i == 0 && input == up)) {
                    int old_x = players[i].x, old_y = players[i].y;
                    // Aumentar la velocidad de 10 a 20 para movimiento más rápido
                    players[i].x += (cos_table[players[i].angle] * 20) / 100;
                    players[i].y += (sin_table[players[i].angle] * 20) / 100;
                    if (players[i].x < PLAYER_RADIUS) players[i].x = PLAYER_RADIUS;
                    if (players[i].x > SCREEN_WIDTH - PLAYER_RADIUS) players[i].x = SCREEN_WIDTH - PLAYER_RADIUS;
                    if (players[i].y < PLAYER_RADIUS + UI_TOP_MARGIN) players[i].y = PLAYER_RADIUS + UI_TOP_MARGIN;
                    if (players[i].y > SCREEN_HEIGHT - PLAYER_RADIUS) players[i].y = SCREEN_HEIGHT - PLAYER_RADIUS;
                    int dx = players[i].x - hole_x, dy = players[i].y - hole_y;
                    if (dx*dx + dy*dy < (PLAYER_RADIUS + 15)*(PLAYER_RADIUS + 15)) {
                        players[i].x = old_x; players[i].y = old_y;
                    }
                } else if ((i == 1 && (input == left || input == left - 32)) || (i == 0 && input == left)) {
                    // Aumentar la velocidad de rotación de 1 a 2 grados para giro más rápido
                    players[i].angle = (players[i].angle + 34) % 36;  // Rotación más rápida (2 grados)
                } else if ((i == 1 && (input == right || input == right - 32)) || (i == 0 && input == right)) {
                    players[i].angle = (players[i].angle + 2) % 36;   // Rotación más rápida (2 grados)
                }

                    // Golpe a la pelota
                    int dx = players[i].ball_x - players[i].x, dy = players[i].ball_y - players[i].y;
                    if (dx*dx + dy*dy <= (PLAYER_RADIUS + 5)*(PLAYER_RADIUS + 5) && players[i].puede_golpear) {
                        int golpe_power = 30;
                        players[i].ball_vx = (cos_table[players[i].angle] * golpe_power) / 10;
                        players[i].ball_vy = (sin_table[players[i].angle] * golpe_power) / 10;
                        players[i].puede_golpear = 0;
                        players[i].golpes++;
                        audiobounce();
                    }
                }
            }
        }

        // Permitir volver a golpear solo si el jugador está lejos de su pelota
        for (int i = 0; i < num_players; i++) {
            int dx = players[i].ball_x - players[i].x, dy = players[i].ball_y - players[i].y;
            if (dx*dx + dy*dy > (PLAYER_RADIUS + 5)*(PLAYER_RADIUS + 5)) players[i].puede_golpear = 1;
        }

        // Física de las pelotas
        for (int i = 0; i < num_players; i++) {
            if ((players[i].ball_vx != 0 || players[i].ball_vy != 0) && !players[i].ball_in_hole) {
                players[i].ball_x += players[i].ball_vx / 10;
                players[i].ball_y += players[i].ball_vy / 10;
                players[i].ball_vx = (players[i].ball_vx * 95) / 100;
                players[i].ball_vy = (players[i].ball_vy * 95) / 100;
                if (players[i].ball_vx < 1 && players[i].ball_vx > -1) players[i].ball_vx = 0;
                if (players[i].ball_vy < 1 && players[i].ball_vy > -1) players[i].ball_vy = 0;
            }
        }

        // Rebote en bordes
        for (int i = 0; i < num_players; i++) {
            if (players[i].ball_x < 5) { players[i].ball_x = 5; players[i].ball_vx = -players[i].ball_vx; }
            if (players[i].ball_x > SCREEN_WIDTH - 5) { players[i].ball_x = SCREEN_WIDTH - 5; players[i].ball_vx = -players[i].ball_vx; }
            if (players[i].ball_y < UI_TOP_MARGIN + 5) { players[i].ball_y = UI_TOP_MARGIN + 5; players[i].ball_vy = -players[i].ball_vy; }
            if (players[i].ball_y > SCREEN_HEIGHT - 5) { players[i].ball_y = SCREEN_HEIGHT - 5; players[i].ball_vy = -players[i].ball_vy; }
        }

        for (int i = 0; i < num_players; i++) {
            if (players[i].golpes >= 6 && !players[i].ball_in_hole && 
                players[i].ball_vx == 0 && players[i].ball_vy == 0) {
                char lose_msg[80];
                if (num_players == 1)
                    sprintf(lose_msg, "GAME OVER! Llegaste a 6 golpes. Presiona Espacio/ENTER para seguir o ESC para salir.");
                else
                    sprintf(lose_msg, "GAME OVER! %s llego a 6 golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", players[i].name);
                displayFullScreenMessage(lose_msg, COLOR_TEXT_BLUE);
                while (1) {
                    char input = 0;
                    if (try_getchar(&input)) {
                        if (input == 27) { clearScreen(); return; }
                        else if (input == ' ' || input == '\n' || input == '\r') {
                            clearScreen();
                            game_start(num_players);
                            return;
                        }
                    }
                    for (volatile int i = 0; i < 100000; i++);
                }
            }
        }

        // Verificar si alguna pelota llegó al hoyo
        for (int i = 0; i < num_players; i++) {
            int hx = players[i].ball_x - hole_x, hy = players[i].ball_y - hole_y;
            if (hx*hx + hy*hy <= 100 && !players[i].ball_in_hole && ganador == -1) {
                // Mensaje de pelota en hoyo (como el original)
                drawFullWidthBar(0, 16, COLOR_BG_BLACK);
                drawTextWithBg(10, 10, "PELOTA EN HOYO!", COLOR_TEXT_WHITE, COLOR_BG_BLACK);
                sleep(500);
                players[i].ball_in_hole = 1;
                players[i].ball_vx = 0;
                players[i].ball_vy = 0;
                players[i].ball_x = hole_x;
                players[i].ball_y = hole_y;
                ganador = i;
            }
        }

        // --- DIBUJO INCREMENTAL ---
        // 1. Dibujamos el hoyo primero (está en el fondo)
        drawCircle(hole_x, hole_y, 15, COLOR_HOLE);
        
        // 2. Dibujamos los jugadores (están encima del hoyo)
        for (int i = 0; i < num_players; i++) {
            drawCircle(players[i].x, players[i].y, PLAYER_RADIUS, players[i].color);
        }
        
        // 3. Dibujamos las pelotas (están encima de los jugadores)
        for (int i = 0; i < num_players; i++) {
            drawCircle(players[i].ball_x, players[i].ball_y, 5, players[i].ball_color);
        }
        
        // 4. Dibujamos las flechas (están encima de todo)
        for (int i = 0; i < num_players; i++) {
            drawPlayerArrow(players[i].x, players[i].y, players[i].angle, hole_x, hole_y, players[i].arrow_color);
        }
        
        // 5. Actualizamos las posiciones previas
        for (int i = 0; i < num_players; i++) {
            players[i].prev_x = players[i].x;
            players[i].prev_y = players[i].y;
            players[i].prev_angle = players[i].angle;
            players[i].prev_ball_x = players[i].ball_x;
            players[i].prev_ball_y = players[i].ball_y;
        }

        // Si hay ganador, mostrar mensaje y esperar acción
        if (ganador != -1) {
            char victory_msg[80];
            if (num_players == 1)
                sprintf(victory_msg, "VICTORIA! Hoyo completado en %d golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", players[0].golpes);
            else
                sprintf(victory_msg, "GANA %s! en %d golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", players[ganador].name, players[ganador].golpes);
            displayFullScreenMessage(victory_msg, COLOR_TEXT_BLUE);
            while (1) {
                char input = 0;
                if (try_getchar(&input)) {
                    if (input == 27) { clearScreen(); return; }
                    else if (input == ' ' || input == '\n' || input == '\r') {
                        clearScreen();
                        game_start(num_players);
                        return;
                    }
                }
                for (volatile int i = 0; i < 100000; i++);
            }
        }
        sleep(16);
    }
}