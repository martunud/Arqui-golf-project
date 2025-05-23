#include <syscalls_lib.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <rtc.h>


#define STDIN 0
#define STDOUT 1

uint64_t syscall_read(int fd, char *buffer, int count) {
    if (fd != STDIN || buffer == NULL || count <= 0){
        return 0;
    }

    uint64_t read = 0;
    char c;

    while (read < count){
        c = keyboard_read_getchar();
        if (c == 0) {
            break; // No hay mas caracteres
        }
        buffer[read++] = c;
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


uint64_t syscall_getTime(char *buffer) {
    if (buffer == NULL) return -1;
    uint8_t hours = getTime(RTC_HOURS);
    uint8_t minutes = getTime(RTC_MINUTES);
    uint8_t seconds = getTime(RTC_SECONDS);
    buffer[0] = '0' + (hours / 10);
    buffer[1] = '0' + (hours % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (minutes / 10);
    buffer[4] = '0' + (minutes % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (seconds / 10);
    buffer[7] = '0' + (seconds % 10);
    buffer[8] = '\0';
    return 8;
}

uint64_t syscall_getRegisters(uint64_t * r) {
    return getRegisters(r);
}