#include "lib.h"
#include "include/syscall.h"
#include <stdarg.h>


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

int strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

char *fgets(char *s, int n, int fd) {
    int i = 0;
    char c;
    int read;

    if (n <= 0) return 0;

    while (i < n - 1) {
        read = sys_read(fd, &c, 1);
        if (read <= 0) {
            if (i == 0) return 0; 
            break;
        }

        s[i++] = c;
        if (c == '\n') break;
    }

    s[i] = '\0';
    return s;
}

void strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;
    while(src[i] != 0 && i < n) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int atoi(char* str){
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    return res;
}

int scanf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char input[BUFFER_SIZE];
    if (!fgets(input, BUFFER_SIZE, stdin)) {
        va_end(args);
        return 0; 
    }

    int index = 0;  // índice para recorrer input
    int count = 0;  // cantidad de valores leídos

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    char *dest = va_arg(args, char *);
                    int start = index;
                    while (input[index] != '\0' && input[index] != '\n' && input[index] != ' ') {
                        index++;
                    }
                    int len = index - start;
                    if (len > 0) {
                        strncpy(dest, input + start, len);
                        dest[len] = '\0';
                        count++;
                    }
                    if (input[index] == ' ') index++;
                    break;
                }
                case 'd': {
                    int *dest = va_arg(args, int *);
                    int start = index;
                    while (input[index] != '\0' && input[index] != '\n' && input[index] != ' ') {
                        index++;
                    }
                    int len = index - start;
                    if (len > 0) {
                        char temp[32];
                        if (len >= (int)sizeof(temp)) len = sizeof(temp) - 1;
                        strncpy(temp, input + start, len);
                        temp[len] = '\0';
                        *dest = atoi(temp);
                        count++;
                    }
                    if (input[index] == ' ') index++;
                    break;
                }
                case 'c': {
                    char *dest = va_arg(args, char *);
                    // Saltear espacios y saltos de línea previos
                    while (input[index] == ' ' || input[index] == '\n')
                        index++;
                    if (input[index] != '\0') {
                        *dest = input[index];
                        index++;
                        count++;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    va_end(args);
    return count;
}


int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int count = 0;

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    char *s = va_arg(args, char *);
                    while (*s) {
                        putchar(*s++);
                        count++;
                    }
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    if (n == 0) {
                        putchar('0');
                        count++;
                        break;
                    }

                    if (n < 0) {
                        putchar('-');
                        n = -n;
                        count++;
                    }

                    char buf[12];
                    int i = 0;

                    while (n > 0) {
                        buf[i++] = (n % 10) + '0';
                        n /= 10;
                    }

                    while (i--) {
                        putchar(buf[i]);
                        count++;
                    }

                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    putchar(c);
                    count++;
                    break;
                }
                default:
                    putchar('%');
                    putchar(fmt[i]);
                    count += 2;
                    break;
            }
        } else {
            putchar(fmt[i]);
            count++;
        }
    }

    va_end(args);
    return count;
}
