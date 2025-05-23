#include "../include/commands.h"
#include "../include/lib.h"
#include "../include/shell.h"

const TShellCmd shellCmds[] = {
    {"help", helpCmd, ": Muestra los comandos disponibles\n"},
    {"exit", exitCmd, ": Salir del shell\n"},
    {"set-user", setUserCmd, ": Setea el nombre de usuario, con un maximo de 10 caracteres\n"},
    {"clear", clearCmd, ": Limpia la pantalla\n"},
    {"time", timeCmd, ": Muestra la hora actual\n"},
    {"font-size", fontSizeCmd, ": Cambia el tamano de la fuente\n"},
    {NULL, NULL, NULL}, // Comando vacío para indicar el final de la lista
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
    char newName[MAX_USER_LENGTH + 1];
    
    printf("%s", "Ingrese el nuevo nombre de usuario: ");
    readLine(newName, sizeof(newName));
    
    strncpy(shellUser, newName, MAX_USER_LENGTH);
    printf("Nombre de usuario actualizado a: %s\n", shellUser);
}

void clearCmd(void){
    clearScreen();
}

void timeCmd(void){
    char time[TIME_BUFF];
    getTime(time);
    printf("Hora del sistema: %s\n", time);
}

void fontSizeCmd(void){
    int size;
    printf("Ingrese el nuevo tamaño de la fuente (1-3): ");
    scanf("%d", &size);
    
    if(size < 1 || size > 3){
        printf("Tamaño inválido. Debe estar entre 1 y 3.\n");
        return;
    }
    
    setFontScale(size);
    clearCmd();
    printf("Tamaño de fuente cambiado a: %d\n", size);
}