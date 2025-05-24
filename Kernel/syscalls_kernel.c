#include <syscalls_lib.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <audioDriver.h>
#include <rtc.h>
#include <stddef.h>
#include <interrupts.h>
#include <time.h>

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

uint64_t syscall_getRegisters(uint64_t *buffer) {
    if (buffer == NULL) {
        return 0;
    }
    
    uint64_t result = getRegisters(buffer);
    if(result == 0) {
        // Si no hay registros cargados, llenar con ceros
        for(int i = 0; i < REGISTERS_CANT; i++) {
            buffer[i] = 0;
        }
    }
    
    return REGISTERS_CANT;  // Retornar cantidad de registros copiados
}

uint64_t syscall_clearScreen() {
    video_clearScreen();
    return 1;
}

uint64_t syscall_beep(int frequency, int duration) {
    audio_play(frequency);
    syscall_sleep(duration);
    audio_stop();
    return 1;
}

uint64_t syscall_sleep(int duration) {

    const uint32_t HZ = 18;
    uint64_t start     = ticks_elapsed();
    uint64_t wait_tics = (duration * HZ + 999) / 1000;
    uint64_t target    = start + wait_tics;

    // cada _hlt() debe habilitar IRQ antes de hlt, por ejemplo:
    // _hlt() ≡ asm { sti; hlt; ret }
    while (ticks_elapsed() < target) {
        _hlt();
    }
    return 0;
}

uint64_t syscall_setFontScale(int scale) {
    if (scale < 1 || scale > 5) {
        return 0; // Tamaño inválido
    }
    
    setFontScale(scale);
    return 1; // Éxito
}