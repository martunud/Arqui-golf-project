#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include <stdint.h>

#define ESC 0x1B
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define SHIFT_L 0x2A
#define SHIFT_R 0x36
#define CTRL_L 0x1D
#define CTRL_L_RELEASE 0x9D
#define CAPSLOCK 0x3A
#define RELEASE_OFFSET 0x80
#define KEY_COUNT 59
#define KEYBOARD_DATA_PORT 0x60
#define MAX_SCANCODE 0x58
#define BUFFER_SIZE 256
#define REGISTERS_CANT 16

static int buffer_empty();
static int buffer_full();
static char buffer_pop();
static char buffer_push(char c);
static char scToAscii(uint8_t scancode);
static void updateFlags(uint8_t scancode);



/* Manejador de interrupciones para el teclado, 
 * se ejecuta cuando el teclado genera una interrupcion
*/
void keyboard_interrupt_handler();

/* 
 * Devuelve un caracter desde el buffer del teclado o 0 si el buffer esta vacio
*/
char keyboard_read_getchar();

/*
 * Llena un arreglo r con los valores actuales de los registros del procesador
*/
uint64_t getRegisters(uint64_t * r);

#endif // KEYBOARD_DRIVER_H