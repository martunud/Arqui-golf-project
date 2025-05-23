#include <syscalls_lib.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <rtc.h>
#include <stddef.h>


#define STDIN 0
#define STDOUT 1

uint64_t syscall_read(int fd, char *buffer, int count) {
    if (fd != STDIN || buffer == NULL || count <= 0){
        return 0;
    }

    uint64_t read = 0;
    char c;

    while (read < count) {
    c = keyboard_read_getchar();
    if (c == 0) break;
    buffer[read++] = c;
    if (c == '\n') break; // Terminar en salto de línea
}

    return read;
}


uint64_t syscall_write(int fd, const char * buffer, int count) {
    if (fd != STDOUT) {
        return 0;
    }
    
    for (int i = 0; i < count; i++) {
        video_putChar(buffer[i], FOREGROUND_COLOR, BACKGROUND_COLOR);
    }
    
    return count;
}


uint64_t syscall_getTime(uint64_t reg) {
    return getTime(reg);
}

uint64_t syscall_getRegisters(uint64_t * r) {
    return getRegisters(r);
}