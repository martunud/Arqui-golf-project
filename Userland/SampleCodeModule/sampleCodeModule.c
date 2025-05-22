/* sampleCodeModule.c */
#include "include/lib.h"


int main(void) {
    putchar('>');           // Muestra un prompt sencillo
    char c = getchar();     // Bloquea hasta que pulses una tecla
    putchar('\n');          // Salto de línea
    putchar('T');           // Mensaje de confirmación
    putchar('u');
    putchar(' ');
    putchar('c');
    putchar('a');
    putchar('r');
    putchar('a');
    putchar('c');
    putchar('t');
    putchar('e');
    putchar('r');
    putchar(':');
    putchar(' ');
    putchar(c);             // Imprime el carácter leído
    putchar('\n');          // Salto final
    return 0;
}
