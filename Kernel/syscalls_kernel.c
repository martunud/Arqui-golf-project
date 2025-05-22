#include <syscalls_lib.h>
#include <keyboardDriver.h>
#include <videoDriver.h>

#define STDIN 0
#define STDOUT 1

uint64_t syscall_read(int fd, char * buffer, int count) {
    // STDIN is the only file descriptor supported so far
    if(fd != STDIN) {
        return 0;
    }
    for(int i = 0; i < count; i++) {
        char c = keyboard_read_getchar();
        if(c == 0) {
            return i;
        }
        buffer[i] = c;
    }
    return count;
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

