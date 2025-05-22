#include "lib.h"
#include "include/syscall.h"

void putchar(char c) {
    sys_write(1, &c, 1);
}

char getchar() {
    char c;
    sys_read(0, &c, 1);
    return c;
}

