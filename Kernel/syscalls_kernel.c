#include <stdio.h>
#include <syscalls_lib.h>
#include <keyboardDriver.h>
#include <videoDriver.h>


#define STDIN 0
#define STDOUT 1

uint64_t syscall_read(int fd, char * buffer, int count) {
    // Validate parameters
    if(fd != STDIN || buffer == NULL || count <= 0) {
        return 0;
    }

    int i = 0;
    while(i < count) {
        // keyboard_read_getchar() returns 0 if buffer is empty
        char c;
        // Wait until we get a character
        while((c = keyboard_read_getchar()) == 0);
        
        // Store the character
        buffer[i] = c;
        i++;

        // If we read Enter, return immediately
        if(c == '\n') {
            return i;
        }
    }

    return i;
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

