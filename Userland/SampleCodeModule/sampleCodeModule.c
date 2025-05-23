/* sampleCodeModule.c */
#include "include/shell.h"
#include "include/lib.h"
#include "include/syscall.h"

#define REGISTERS_CANT 17

void test_registers(void);


//void test_time();

int main(void) {

    test_registers();

    //test_time();

    //shellLoop();

    // putchar('>');           // Muestra un prompt sencillo
    // char c = getchar();     // Bloquea hasta que pulses una tecla
    // putchar('\n');          // Salto de línea
    // putchar('T');           // Mensaje de confirmación
    // putchar('u');
    // putchar(' ');
    // putchar('c');
    // putchar('a');
    // putchar('r');
    // putchar('a');
    // putchar('c');
    // putchar('t');
    // putchar('e');
    // putchar('r');
    // putchar(':');
    // putchar(' ');
    // putchar(c);             // Imprime el carácter leído
    // putchar('\n');          // Salto final
    
    // char str[100];
    // int num;

    // printf("Enter a string: ");
    // scanf("%s", str);
    // printf("You entered: %s\n", str);

    // printf("Enter a number: ");
    // scanf("%d", &num);
    // printf("You entered: %d\n", num);

    // printf("Enter a character: ");
    // scanf("%c", &c);
    // printf("You entered: %c\n", c);

    return 0;
}

/*
Crea un buffer para almacenar los 17 registros (REGISTERS_CANT)
Muestra el mensaje "Press r to capture registers..."
Cuando el usuario presiona r:
La syscall captura el estado actual de los registros
Devuelve la cantidad de registros capturados
Si se capturaron registros (count > 0):
Muestra cada registro en formato hexadecimal
Imprime nombre del registro (RAX, RBX, etc.) y su valor
Por ejemplo: RAX: 0x0000000000000001
Si no se capturaron registros:
Muestra un mensaje de error
*/
void test_registers(void) {
    uint64_t regs[REGISTERS_CANT];
    
    // 1. Imprimir mensaje inicial
    sys_write(1, "Press 'r' to capture registers (Ctrl+Alt+G to exit QEMU)\n", 55);
    
    // 2. Esperar la tecla 'r'
    char c;
    while(1) {
        if(sys_read(0, &c, 1) > 0) {
            if(c == 'r') break;
        }
    }
    
    // 3. Intentar capturar registros
    int count = sys_getRegisters(regs);
    
    // 4. Verificar resultado
    if(count > 0) {
        char *reg_names[] = {
            "RAX", "RBX", "RCX", "RDX", 
            "RSI", "RDI", "RBP", "R8", 
            "R9", "R10", "R11", "R12", 
            "R13", "R14", "R15", "RIP", 
            "RSP"
        };
        
        sys_write(1, "\nRegisters captured successfully:\n", 32);
        
        // Imprimir cada registro
        char buffer[50];
        for(int i = 0; i < REGISTERS_CANT; i++) {
            // Formato manual: "REG: 0xVALUE\n"
            int pos = 0;
            
            // Copiar nombre del registro
            char *name = reg_names[i];
            while(*name) buffer[pos++] = *name++;
            
            // Agregar ": 0x"
            buffer[pos++] = ':';
            buffer[pos++] = ' ';
            buffer[pos++] = '0';
            buffer[pos++] = 'x';
            
            // Convertir valor a hex
            for(int j = 15; j >= 0; j--) {
                int digit = (regs[i] >> (j * 4)) & 0xF;
                buffer[pos++] = digit < 10 ? '0' + digit : 'A' + (digit - 10);
            }
            
            buffer[pos++] = '\n';
            
            sys_write(1, buffer, pos);
        }
    } else {
        sys_write(1, "\nError: Could not capture registers\n", 34);
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




