#ifndef SYSCALLS_LIB_H
#define SYSCALLS_LIB_H

#include <stdint.h>
#include "registers.h"



uint64_t syscall_get_regs(uint64_t *dest);

/** fd: Un entero que representa el descriptor de archivo desde el cual se leerán los datos.
 * buffer: Un puntero a un búfer en memoria donde se almacenarán los datos leídos
 * count: indica el número máximo de bytes que se leerán del descriptor de archivo. La función read intentará leer hasta ese número de bytes, pero puede que lea menos si se llega al final del archivo u ocurre un error
*/ 
uint64_t syscall_read(int fd, char * buffer, int count);


/** fd: Un entero que identifica el archivo o dispositivo en el que se escribiran los datos.
 * buffer: Un puntero a una región de memoria que contiene los datos que se van a escribir
 * count: Un entero que indica la cantidad de bytes que se van a escribir desde el buffer
 */
uint64_t syscall_write(int fd, const char * buffer, int count);


/*
 * arg: un entero que representa el argumento que se le pasará a la función getTime
*/
uint64_t syscall_getTime(uint64_t reg);


/*
 * Limpiar la pantalla
 * Retorna: 1 si se ejecuta correctamente
 */
uint64_t syscall_clearScreen();

/*
 * arg: un entero que representa la frecuencia y el tiempo de la señal
 * duration: tiempo en milisegundos
*/
uint64_t syscall_beep(int frequency, int duration);

/*
 * arg: un entero que representa la duración de la pausa en milisegundos
*/
uint64_t syscall_sleep(int duration);

uint64_t syscall_setFontScale(int scale);


uint64_t syscall_video_putPixel(uint64_t x, uint64_t y, uint64_t color, uint64_t unused1, uint64_t unused2);
uint64_t syscall_video_putChar(uint64_t c, uint64_t fg, uint64_t bg, uint64_t unused1, uint64_t unused2);
uint64_t syscall_video_clearScreenColor(uint64_t color, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4);
uint64_t syscall_video_putCharXY(uint64_t c, uint64_t x, uint64_t y, uint64_t fg, uint64_t bg);

/*
 * Verifica si una tecla específica está presionada
 * scancode: el código de escaneo de la tecla a verificar
 * Retorna: 1 si la tecla está presionada, 0 si no
 */
uint64_t syscall_is_key_pressed(uint64_t scancode);

/*
 * Apaga el sistema (detiene la ejecución del CPU)
 * No recibe parámetros
 * No retorna
 */
uint64_t syscall_shutdown(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5);

#endif