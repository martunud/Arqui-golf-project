#include "../include/commands.h"
#include "../include/lib.h"

const TShellCmd shellCmds[] = {
    {"help", helpCmd, ": Muestra los comandos disponibles\n"},
    {"exit", exitCmd, ": Salir del shell\n"},
    {"set-user", setUserCmd, ": Setea el nombre de usuario, con un maximo de 10 caracteres\n"},
    {NULL, NULL, NULL}
};


void helpCmd(void){
    printf("%s", "Comandos disponibles:\n");
    for(int i = 0; shellCmds[i].name; i++){
        printf("%s", shellCmds[i].name);
        printf("%s", shellCmds[i].help);
    }
}

void exitCmd(void){
    printf("%s", "Saliendo del shell...\n");
}

void setUserCmd(void){
    return;
}