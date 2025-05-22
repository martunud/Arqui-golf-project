#include "lib.h"
#include "include/syscall.h"

void putchar(char c) {
    sys_write(1, &c, 1);
}

char getchar(void) {
    char c;
    int n;

    do{
        n = sys_read(0, &c, 1);
    }while (n == 0);

    return c;
}


