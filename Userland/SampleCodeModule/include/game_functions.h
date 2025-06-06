#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include <stdint.h>

// Colores
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

#define FRICTION 95         
#define MIN_VELOCITY 1      
#define BOUNCE_FACTOR 90    
#define POWER_FACTOR 20

extern const int cos_table[36];
extern const int sin_table[36];

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
    int debe_verificar_derrota;
    char control_up, control_left, control_right;
    char *name;
} Player;

void drawCircle(int cx, int cy, int radius, uint32_t color);
void drawText(int x, int y, const char *text, uint32_t color);
void drawTextWithBg(int x, int y, const char *text, uint32_t textColor, uint32_t bgColor);
void eraseBallSmart(int prev_ball_x, int prev_ball_y, Player *players, int num_players, int hole_x, int hole_y);
void erasePlayerSmart(int prev_x, int prev_y, Player *players, int num_players, int hole_x, int hole_y);
int isInsideHole(int x, int y, int h_x, int h_y);
unsigned int rand();
int rand_range(int min, int max);
int circles_overlap(int x1, int y1, int r1, int x2, int y2, int r2);
void drawFullWidthBar(int y, int height, uint32_t color);
void displayFullScreenMessage(const char *message, uint32_t textColor);
void drawPlayerArrow(int player_x, int player_y, int player_angle, int hole_x, int hole_y, int arrow_color, Player *players, int num_players);
void eraseArrow(int prev_x, int prev_y, int prev_angle, int hole_x, int hole_y, Player *players, int num_players);
void drawTextFixed(int x, int y, const char *text, uint32_t color, uint32_t bg);
void audiobounce();
#endif
