#include "include/lib.h"
#include "include/game.h"
#include "include/game_functions.h"


// Pantalla principal del juego
void game_main_screen() {
    init_screen_dimensions();
    video_clearScreenColor(COLOR_BG_HOME);
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
    int total_height = num_lines * line_height;
    int startY = (SCREEN_HEIGHT - total_height) / 2;
    for (int i = 0; i < num_lines; i++) {
        int text_len = strlen(lines[i]);
        int text_width = text_len * font_width;
        int x = (SCREEN_WIDTH - text_width) / 2;
        int y = startY + i * line_height;
        drawText(x, y, lines[i], COLOR_TEXT_HOME);
    }
    while (1) {
        if (is_key_pressed_syscall((unsigned char)SC_1)) {
            game_start(1);
            break;
        } else if (is_key_pressed_syscall((unsigned char)SC_2)) {
            game_start(2);
            break;
        } else if (is_key_pressed_syscall((unsigned char)SC_ESC)) {
            clearScreen();
            return;
        }
        for (volatile int i = 0; i < 100000; i++);
    }
}

// Lógica principal del juego
void game_start(int num_players) {
    video_clearScreenColor(COLOR_BG_GREEN);
    int margin = 50;
    int hole_x = rand_range(margin + 15, SCREEN_WIDTH - margin - 15);
    int hole_y = rand_range(UI_TOP_MARGIN + margin + 15, SCREEN_HEIGHT - margin - 15);
    Player players[2] = {0};

    // Configuración de jugadores
    players[0].color = COLOR_PLAYER1;
    players[0].arrow_color = COLOR_TEXT_HOME;
    players[0].ball_color = COLOR_BALL1;
    players[0].control_up = SC_UP;     // Usar scancode para flecha arriba
    players[0].control_left = SC_LEFT;  // Usar scancode para flecha izquierda
    players[0].control_right = SC_RIGHT; // Usar scancode para flecha derecha
    players[0].name = "Blanco";

    if (num_players == 2) {
        players[1].color = COLOR_PLAYER2;
        players[1].arrow_color = COLOR_TEXT_HOME;
        players[1].ball_color = COLOR_BALL2;
        players[1].control_up = SC_W;     // Usar scancode para W
        players[1].control_left = SC_A;   // Usar scancode para A
        players[1].control_right = SC_D;  // Usar scancode para D
        players[1].name = "Azul";
    }

    // Inicialización de posiciones
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
        players[i].debe_verificar_derrota = 0;
    }

    // Dibujo inicial
    drawCircle(hole_x, hole_y, 15, COLOR_BLACK);
    for (int i = 0; i < num_players; i++) {
        drawCircle(players[i].x, players[i].y, PLAYER_RADIUS, players[i].color);
        drawCircle(players[i].ball_x, players[i].ball_y, 5, players[i].ball_color);
        drawPlayerArrow(players[i].x, players[i].y, players[i].angle, hole_x, hole_y, players[i].arrow_color, players, num_players);
    }

    drawFullWidthBar(0, 16, COLOR_BLACK);
    drawFullWidthBar(16, 16, COLOR_BLACK);

    int last_golpes_p1 = -1, last_golpes_p2 = -1;
    char status_str[80];

    if (num_players == 1) {
        sprintf(status_str, "Golpes: %d, Modo: Solitario", players[0].golpes);
        drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BLACK);
        last_golpes_p1 = players[0].golpes;
    } else {
        sprintf(status_str, "P1: %d | P2: %d   ESC: salir", players[0].golpes, players[1].golpes);
        drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BLACK);
        last_golpes_p1 = players[0].golpes;
        last_golpes_p2 = players[1].golpes;
    }

    int ganador = -1;
    while (1) {
        if (num_players == 1) {
            if (players[0].golpes != last_golpes_p1) {
                sprintf(status_str, "Golpes: %d, Modo: Solitario", players[0].golpes);
                drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BLACK);
                last_golpes_p1 = players[0].golpes;
            }
        } else {
            if (players[0].golpes != last_golpes_p1 || players[1].golpes != last_golpes_p2) {
                sprintf(status_str, "P1: %d | P2: %d   ESC: salir", players[0].golpes, players[1].golpes);
                drawTextFixed(10, 0, status_str, COLOR_TEXT_WHITE, COLOR_BLACK);
                last_golpes_p1 = players[0].golpes;
                last_golpes_p2 = players[1].golpes;
            }
        }

        // Control de los jugadores usando el nuevo sistema de detección de teclas
        if (is_key_pressed_syscall((unsigned char)SC_ESC)) { 
            clear_key_buffer(); // Limpiar buffer antes de salir
            clearScreen(); 
            break; 
        }
        
        // Control de los jugadores
        for (int i = 0; i < num_players; i++) {
            if (!players[i].ball_in_hole) {
                // Mover hacia adelante
                if (is_key_pressed_syscall((unsigned char)players[i].control_up)) {
                    int new_x = players[i].x + (cos_table[players[i].angle] * 6) / 100;
                    int new_y = players[i].y + (sin_table[players[i].angle] * 6) / 100;
                    // Verifica límites de pantalla
                    if (new_x >= PLAYER_RADIUS && new_x <= SCREEN_WIDTH - PLAYER_RADIUS &&
                        new_y >= UI_TOP_MARGIN + PLAYER_RADIUS && new_y <= SCREEN_HEIGHT - PLAYER_RADIUS) {
                        // Verifica que no pise el hoyo
                        int dx = new_x - hole_x;
                        int dy = new_y - hole_y;
                        if (dx*dx + dy*dy > (PLAYER_RADIUS + 15)*(PLAYER_RADIUS + 15)) {
                            players[i].x = new_x;
                            players[i].y = new_y;
                        }
                    }
                }
                
                // Rotar a la izquierda - más lento
                if (is_key_pressed_syscall((unsigned char)players[i].control_left)) {
                    players[i].angle = (players[i].angle + 35) % 36;
                    // Nota: No usamos sleep aquí para no bloquear la detección de otras teclas
                    // La velocidad de rotación se controla con el sleep al final del bucle principal
                }
                
                // Rotar a la derecha - más lento
                if (is_key_pressed_syscall((unsigned char)players[i].control_right)) {
                    players[i].angle = (players[i].angle + 1) % 36;
                    // Nota: No usamos sleep aquí para no bloquear la detección de otras teclas
                    // La velocidad de rotación se controla con el sleep al final del bucle principal
                }
            }
        }

        // Detecta colisiones entre jugadores y pelotas (golpe)
        for (int i = 0; i < num_players; i++) {
            for (int j = 0; j < num_players; j++) {
                if (!players[j].ball_in_hole) {
                    int dx = players[j].ball_x - players[i].x;
                    int dy = players[j].ball_y - players[i].y;
                    int dist_squared = dx*dx + dy*dy;
                    if (dist_squared <= (PLAYER_RADIUS + 5)*(PLAYER_RADIUS + 5) && players[i].puede_golpear) {
                        // Si el jugador intenta un 7mo toque, pierde instantáneamente
                        if (i == j && players[i].golpes >= 6 && ganador == -1) {
                            ganador = (i == 0) ? 1 : 0; // El otro jugador gana
                            players[i].debe_verificar_derrota = 1;
                            // Mostrar mensaje de derrota inmediatamente
                            char msg[120];
                            if (num_players == 1) {
                                sprintf(msg, "DERROTA! No lograste meter la pelota en 6 golpes. Presiona Espacio/ENTER para reintentar o ESC para salir.");
                            } else {
                                int perdedor = i;
                                int ganador_idx = (i == 0) ? 1 : 0;
                                sprintf(msg, "GANA %s! %s no logró meter la pelota en 6 golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", players[ganador_idx].name, players[perdedor].name);
                            }
                            displayFullScreenMessage(msg, COLOR_TEXT_HOME);
                            
                            // Reproducir sólo una nota como indicación sonora
                            beep(392, 250);
                            
                            // Pequeño retraso para evitar detección inmediata de teclas
                            sleep(100);
                            
                            // Limpiar cualquier tecla presionada antes de entrar al bucle
                            clear_key_buffer();
                            
                            while (1) {
                                // Verificamos las teclas
                                if (is_key_pressed_syscall((unsigned char)SC_ESC)) { 
                                    clear_key_buffer();
                                    clearScreen(); 
                                    return; 
                                }
                                else if (is_key_pressed_syscall((unsigned char)SC_SPACE) || 
                                         is_key_pressed_syscall((unsigned char)SC_ENTER)) {
                                    clearScreen();
                                    game_start(num_players);
                                    return;
                                }
                                
                                sleep(30); // Esperar menos tiempo para mejor respuesta
                            }
                        }
                        players[j].ball_vx = (cos_table[players[i].angle] * POWER_FACTOR) / 10;
                        players[j].ball_vy = (sin_table[players[i].angle] * POWER_FACTOR) / 10;
                        if (i == j && players[i].golpes < 6) {
                            players[i].golpes++;
                        }
                        players[i].puede_golpear = 0;
                        audiobounce();
                    }
                }
            }
        }

        for (int i = 0; i < num_players; i++) {
            int dx = players[i].ball_x - players[i].x;
            int dy = players[i].ball_y - players[i].y;
            if (dx*dx + dy*dy > (PLAYER_RADIUS + 5)*(PLAYER_RADIUS + 5)) 
                players[i].puede_golpear = 1;
        }

        //FISICA DE LA PELOTA 
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
            if (players[i].ball_x < 5) { players[i].ball_x = 5; players[i].ball_vx = -players[i].ball_vx; audiobounce(); }
            if (players[i].ball_x > SCREEN_WIDTH - 5) { players[i].ball_x = SCREEN_WIDTH - 5; players[i].ball_vx = -players[i].ball_vx; audiobounce(); }
            if (players[i].ball_y < UI_TOP_MARGIN + 5) { players[i].ball_y = UI_TOP_MARGIN + 5; players[i].ball_vy = -players[i].ball_vy; audiobounce(); }
            if (players[i].ball_y > SCREEN_HEIGHT - 5) { players[i].ball_y = SCREEN_HEIGHT - 5; players[i].ball_vy = -players[i].ball_vy; audiobounce(); }
        }
        // Verificar si alguna pelota llegó al hoyo
        for (int i = 0; i < num_players; i++) {
            int hx = players[i].ball_x - hole_x, hy = players[i].ball_y - hole_y;
            if (hx*hx + hy*hy <= 15*15 && !players[i].ball_in_hole && ganador == -1) {
                players[i].ball_in_hole = 1;
                players[i].ball_vx = 0;
                players[i].ball_vy = 0;
                players[i].ball_x = hole_x;
                players[i].ball_y = hole_y;
                ganador = i;
            }
        }

        for (int i = 0; i < num_players; i++) {
            if (!players[i].ball_in_hole && players[i].golpes >= 6 && 
                players[i].ball_vx == 0 && players[i].ball_vy == 0 && 
                ganador == -1) {
                if (num_players == 1) {
                    ganador = -2;
                } 
                else {
                    ganador = (i == 0) ? 1 : 0;
                }
            }
        }

        drawCircle(hole_x, hole_y, 15, COLOR_BLACK);
        for (int i = 0; i < num_players; i++) {
            int playerMoved = (players[i].prev_x != players[i].x || players[i].prev_y != players[i].y);
            int ballMoved = (players[i].prev_ball_x != players[i].ball_x || players[i].prev_ball_y != players[i].ball_y);
            int arrowChanged = (!players[i].ball_in_hole && (players[i].prev_angle != players[i].angle || playerMoved));
            if (playerMoved) {
                erasePlayerSmart(players[i].prev_x, players[i].prev_y, players, num_players, hole_x, hole_y);
                drawCircle(players[i].x, players[i].y, PLAYER_RADIUS, players[i].color);
            }
            if (ballMoved) {
                eraseBallSmart(players[i].prev_ball_x, players[i].prev_ball_y, players, num_players, hole_x, hole_y);
                drawCircle(players[i].ball_x, players[i].ball_y, 5, players[i].ball_color);
            }
            if (arrowChanged) {
                eraseArrow(players[i].prev_x, players[i].prev_y, players[i].prev_angle, hole_x, hole_y, players, num_players);
                drawPlayerArrow(players[i].x, players[i].y, players[i].angle, hole_x, hole_y, players[i].arrow_color, players, num_players);
            }
            players[i].prev_x = players[i].x;
            players[i].prev_y = players[i].y;
            players[i].prev_angle = players[i].angle;
            players[i].prev_ball_x = players[i].ball_x;
            players[i].prev_ball_y = players[i].ball_y;
        }

        // Si hay ganador
        if (ganador != -1) {
            char victory_msg[120];
            if (num_players == 1) {
                if (ganador == -2) { 
                    sprintf(victory_msg, "DERROTA! No lograste meter la pelota en 6 golpes. Presiona Espacio/ENTER para reintentar o ESC para salir.");
                } else if (players[0].ball_in_hole) {
                    sprintf(victory_msg, "VICTORIA! Hoyo completado en %d golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", players[0].golpes);
                }
            } else {
                if (players[ganador].ball_in_hole) {
                    sprintf(victory_msg, "GANA %s! Logro meter la pelota en %d golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", 
                        players[ganador].name, players[ganador].golpes);
                } else {
                    int perdedor = (ganador == 0) ? 1 : 0;
                    sprintf(victory_msg, "GANA %s! %s no logro meter la pelota en 6 golpes. Presiona Espacio/ENTER para seguir o ESC para salir.", 
                        players[ganador].name, players[perdedor].name);
                }
            }
            displayFullScreenMessage(victory_msg, COLOR_TEXT_HOME);
            
            
            //beep(G, 250);
            play_mission_impossible();
            // Pequeño retraso para evitar detección inmediata de teclas
            sleep(100);
            
            // Limpiar cualquier tecla presionada antes de entrar al bucle
            clear_key_buffer();
            
            while (1) {
                // Primero verificamos las teclas
                if (is_key_pressed_syscall((unsigned char)SC_ESC)) { 
                    clear_key_buffer();
                    clearScreen(); 
                    return; 
                }
                else if (is_key_pressed_syscall((unsigned char)SC_SPACE) || 
                         is_key_pressed_syscall((unsigned char)SC_ENTER)) {
                    clearScreen();
                    game_start(num_players);
                    return;
                }
                
                sleep(30); // Esperar menos tiempo para mejor respuesta
            }
        }

        // Limitamos la velocidad del juego para que no vaya demasiado rápido
        // Controla tanto la velocidad del juego como la sensibilidad de rotación
        sleep(25); // Este valor controla la velocidad general del juego
    }
}
