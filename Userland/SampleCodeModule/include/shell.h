#ifndef SHELL_H
#define SHELL_H

#define MAX_LINE_LENGTH 100
#define WELCOME_MESSAGE "Bienvenido al shell que no hace nada\n"
#define NOT_FOUND_MESSAGE "Comando no encontrado, escriba help para ver los comandos disponibles\n"
#define FOREGROUND_COLOR 0xFFFFFF
#define BACKGROUND_COLOR 0x000000

extern const char *shellUser;

void shellLoop();

void shellPrompt();

#endif
