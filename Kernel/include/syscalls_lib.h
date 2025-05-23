#ifndef SYSCALLS_LIB_H
#define SYSCALLS_LIB_H

#include <stdint.h>


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
 * Obtiene los valores de los registros del sistema
 * buffer: arreglo donde se guardarán los valores de los registros
 * Retorna: cantidad de registros copiados o 0 si hubo error
 * Orden de los registros: RAX, RBX, RCX, RDX, RSI, RDI, RBP, R8-R15, RIP, RSP
 */
uint64_t syscall_getRegisters(uint64_t * buffer);

#endif