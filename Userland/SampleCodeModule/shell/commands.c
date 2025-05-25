#include "../include/commands.h"
#include "../include/lib.h"
#include "../include/shell.h"

extern void _invalidOp();


static const char * regNames[REGISTERS_CANT] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RSP"
};


const TShellCmd shellCmds[] = {
    {"help", helpCmd, ": Muestra los comandos disponibles\n"},
    {"exit", exitCmd, ": Salir del shell\n"},
    {"set-user", setUserCmd, ": Setea el nombre de usuario, con un maximo de 10 caracteres\n"},
    {"clear", clearCmd, ": Limpia la pantalla\n"},
    {"time", timeCmd, ": Muestra la hora actual\n"},
    {"font-size", fontSizeCmd, ": Cambia el tamano de la fuente\n"},
    {"regs", regsCmd, ": Muestra el estado de los registros (Presione CTRL R para actualizar)\n"},
    {"exceptions", exceptionCmd, ": Testear excepciones. Ingrese: exceptions [zero/invalidOpcode] para testear alguna operacion\n"},
    {NULL, NULL, NULL}, // Comando vacío para indicar el final de la lista
};


int helpCmd(int argc, char *argv[]){
    printf("%s", "Comandos disponibles:\n");
    for(int i = 0; shellCmds[i].name; i++){
        printf("%s", shellCmds[i].name);
        printf("%s", shellCmds[i].help);
    }
    return OK;
}

int exitCmd(int argc, char *argv[]){
    clearScreen();
    printf("%s", "Saliendo del shell...\n");
    return EXIT_CODE;    
}

int setUserCmd(int argc, char *argv[]){
    char newName[MAX_USER_LENGTH + 1];
    
    printf("%s", "Ingrese el nuevo nombre de usuario: ");
    readLine(newName, sizeof(newName));
    
    strncpy(shellUser, newName, MAX_USER_LENGTH);
    printf("Nombre de usuario actualizado a: %s\n", shellUser);
    return OK;
}

int clearCmd(int argc, char *argv[]){
    clearScreen();
    return OK;
}

int timeCmd(int argc, char *argv[]){
    char time[TIME_BUFF];
    getTime(time);
    printf("Hora del sistema: %s\n", time);
    return OK;
}

int fontSizeCmd(int argc, char *argv[]){
    int size;
    printf("Ingrese el nuevo tamaño de la fuente (1-3): ");
    scanf("%d", &size);
    
    if(size < 1 || size > 3){
        printf("Tamaño inválido. Debe estar entre 1 y 3.\n");
        return ERROR;
    }
    
    setFontScale(size);
    clearScreen();
    printf("Tamaño de fuente cambiado a: %d\n", size);
    return OK;
}

int regsCmd(int argc, char *argv[]) {
    uint64_t regs[REGISTERS_CANT];
    int ok = getRegisters(regs);

    if (!ok) {
        printf("[inforeg] Registers are not updated. Use CTRL + R to update.\n");
        return ERROR;
    }

    for (int i = 0; i < REGISTERS_CANT; i += 2) {
        printf("%s: %llx\t", regNames[i], (unsigned long long)regs[i]);

        if (i < (REGISTERS_CANT - 1)) {
            printf("%s: %llx\n", regNames[i + 1], (unsigned long long)regs[i + 1]);
        } else {
            putchar('\n');
            printf("Press CTRL + R to refresh registers.\n");
        }
    }
    return OK;
}
int CommandParse(char *commandInput){
    if(commandInput == NULL)
        return ERROR;
    
    char *args[MAX_ARGS];
    int argc = fillCommandAndArgs(args, commandInput);

    if(argc == 0)
        return ERROR;

    for(int i = 0; shellCmds[i].name; i++) {
        if(strcmp(args[0], shellCmds[i].name) == 0) {
            return shellCmds[i].function(argc, args);
        }
    }

    return ERROR;
}

int fillCommandAndArgs(char *args[], char *input) {
    int argc = 0;
    int inArg = 0;

    for (int i = 0; input[i] != '\0' && argc < MAX_ARGS; i++) {
        if (input[i] == ' ') {
            input[i] = '\0';
            inArg = 0;
        } else if (!inArg) {
            args[argc++] = &input[i];
            inArg = 1;
        }
    }

    return argc;
}

int exceptionCmd(int argc, char * argv[]) {
    if (argc != 2 || argv[1] == NULL) {
        printf("Error: cantidad invalida de argumentos.\nUso: exceptions [zero, invalidOpcode]\n");
        return ERROR;
    }

    if (strcmp(argv[1], "zero") == 0) {
        int a = 1;
        int b = 0;
        int c = a / b;   // Provoca división por cero
        printf("c: %d\n", c); // Esta línea no se ejecutará si hay excepción
    
    }
else if (strcmp(argv[1], "invalidOpcode") == 0) {
    printf("Ejecutando invalidOpcode...\n");
    _invalidOp();    // Provoca opcode inválido
}

    else {
        printf("Error: tipo de excepcion invalido.\nIngrese exceptions [zero, invalidOpcode] para testear alguna operacion\n");
        return ERROR;
    }

    return OK;
}
