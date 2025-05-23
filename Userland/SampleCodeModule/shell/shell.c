#include "../include/shell.h"
#include "../include/commands.h"
#include "../include/lib.h"
#include <videoDriver.h> //despues hay que sacarlo

static void shellPrompt();

const char *shellUser = "usuario";

void shellLoop(){

    char buffer[MAX_LINE_LENGTH];

    video_putString(WELCOME_MESSAGE, 0xFFFFFF, 0x000000); 

    while(1){
        shellPrompt();
        //scanf(buffer, MAX_LINE_LENGTH);

        if(strcmp(buffer, "exit") == 0){
            break;
        }

        int found = 0;

        for(int i = 0; shellCmds[i].name; i++){
            if(strcmp(buffer, shellCmds[i].name) == 0){

                if(strcmp(buffer, "exit") == 0){
                    return;
                }

                shellCmds[i].function();
                found = 1;
                break;
            }
        }

        if(!found){{
            video_putString(NOT_FOUND_MESSAGE, FOREGROUND_COLOR, BACKGROUND_COLOR);
        }}
    }
}

static void shellPrompt(){
    video_putString("TP-ARQUI-", FOREGROUND_COLOR, BACKGROUND_COLOR);
    video_putString(shellUser, FOREGROUND_COLOR, BACKGROUND_COLOR);
    video_putString(":~$ ", FOREGROUND_COLOR, BACKGROUND_COLOR);
}