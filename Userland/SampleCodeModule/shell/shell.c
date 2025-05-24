#include "../include/shell.h"
#include "../include/commands.h"
#include "../include/lib.h"
#include "../include/syscall.h"
static void shellPrompt();

char shellUser[MAX_USER_LENGTH + 1] = "usuario";


void shellLoop(){

    char buffer[MAX_LINE_LENGTH];

    printf("%s", WELCOME_MESSAGE);

    while(1){
        shellPrompt();
        
        readLine(buffer, MAX_LINE_LENGTH);

        size_t len = strlen(buffer);

        if(len == 0){
            continue;
        }

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

void readLine(char *buf, int maxLen) {
    int len = 0;
    char c;

    while (1) {
        c = getchar();          // esto ya bloquea hasta que haya un char
        if (c == '\n') {
            putchar('\n');
            break;
        }
        if (c == '\b') {
            if (len > 0) {
                len--;
                // borra un caracter en pantalla
                putchar('\b'); putchar(' '); putchar('\b');
            }
            else{
                sys_beep(500, 100);
            }
        } else if (len < maxLen-1) {
            buf[len++] = c;
            putchar(c);
        }
    }
    buf[len] = '\0';
}

static void shellPrompt(){

    printf("%s", "TP-ARQUI-");
    printf("%s", shellUser);
    printf("%s", ":~$ ");
    
}