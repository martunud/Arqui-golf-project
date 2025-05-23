#include "../include/shell.h"
#include "../include/commands.h"
#include "../include/lib.h"

static void shellPrompt();

const char *shellUser = "usuario";

void shellLoop(){

    char buffer[MAX_LINE_LENGTH];

    printf("%s", WELCOME_MESSAGE);

    while(1){
        shellPrompt();

        if(fgets(buffer, MAX_LINE_LENGTH, stdin) == NULL){
            printf("%s", "Error al leer la linea\n");
            continue;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        if (strcmp(buffer, "exit") == 0) {
            return;
        }


        int found = 0;

        for(int i = 0; shellCmds[i].name; i++){
            if(strcmp(buffer, shellCmds[i].name) == 0){
                shellCmds[i].function();
                found = 1;
                break;
            }
        }

        if(!found){
            printf("%s", NOT_FOUND_MESSAGE);
        }
    }
}

static void shellPrompt(){

    printf("%s", "TP-ARQUI-");
    printf("%s", shellUser);
    printf("%s", ":~$ ");
}
