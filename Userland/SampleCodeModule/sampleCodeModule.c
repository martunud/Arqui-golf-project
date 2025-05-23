/* sampleCodeModule.c */
<<<<<<< Updated upstream
#include "include/lib.h"
#include "include/syscall.h"

#define REGISTERS_CANT 17

void test_registers();

//void test_time();

int main(void) {

    test_registers();

    //test_time();

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

/*
Crea un buffer para almacenar los 17 registros (REGISTERS_CANT)
Muestra el mensaje "Press Ctrl+R to capture registers..."
Cuando el usuario presiona Ctrl+R:
La syscall captura el estado actual de los registros
Devuelve la cantidad de registros capturados
Si se capturaron registros (count > 0):
Muestra cada registro en formato hexadecimal
Imprime nombre del registro (RAX, RBX, etc.) y su valor
Por ejemplo: RAX: 0x0000000000000001
Si no se capturaron registros:
Muestra un mensaje de error
*/

void test_registers() {
    uint64_t regs[REGISTERS_CANT];
    
    putchar('\n');
    char *msg = "Press Ctrl+R to capture registers...\n";
    while(*msg) putchar(*msg++);
    
    // Llamar a syscall
    int count = sys_getRegisters(regs);
    
    if(count > 0) {
        char *msg2 = "Registers captured:\n";
        while(*msg2) putchar(*msg2++);
        
        char *reg_names[] = {"RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", 
                           "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
                           "RIP", "RSP"};
        
        // Print each register
        for(int i = 0; i < count; i++) {
            // Print register name
            char *name = reg_names[i];
            while(*name) putchar(*name++);
            putchar(':');
            putchar(' ');
            
            // Print hex value
            char hex[17];
            hex[16] = 0;
            uint64_t val = regs[i];
            for(int j = 15; j >= 0; j--) {
                int digit = val & 0xF;
                hex[j] = digit < 10 ? '0' + digit : 'A' + (digit - 10);
                val >>= 4;
            }
            putchar('0');
            putchar('x');
            char *hexPtr = hex;
            while(*hexPtr) putchar(*hexPtr++);
            putchar('\n');
        }
    } else {
        char *error = "No registers captured. Make sure to press Ctrl+R first.\n";
        while(*error) putchar(*error++);
    }
}

/*
Crea un buffer de 9 bytes para almacenar la hora (8 caracteres + null terminator)
Llama a sys_getTime() que:
Lee la hora actual del RTC
La formatea en formato "HH:MM:SS"
Muestra "Current time: " seguido de la hora actual
El resultado sería algo como: Current time: 14:30:45

*/

/*
void test_time() {
    char time_buffer[9];  // 8 chars for HH:MM:SS + null terminator
    
    // Get and display the time
    sys_getTime(time_buffer);
    
    char *msg = "Current time: ";
    while(*msg) putchar(*msg++);
    
    // Print the time string
    for(int i = 0; time_buffer[i] != '\0'; i++) {
        putchar(time_buffer[i]);
    }
    putchar('\n');
}
*/
=======
#include "include/lib.h" 

int main(void) {
    char str[100];
    int num;
    char c;

    printf("Enter a string: ");
    scanf("%s", str);
    printf("You entered: %s\n", str);

    printf("Enter a number: ");
    scanf("%d", &num);
    printf("You entered: %d\n", num);

    printf("Enter a character: ");
    scanf("%c", &c);
    printf("You entered: %c\n", c);

    return 0;
}

>>>>>>> Stashed changes
