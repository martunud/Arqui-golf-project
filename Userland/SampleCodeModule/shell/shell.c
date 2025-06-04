#include "../include/shell.h"
#include "../include/commands.h"
#include "../include/lib.h"
#include "../include/syscall.h"
static void shellPrompt();

char shellUser[MAX_USER_LENGTH + 1] = "usuario";
void shellLoop() {
    char buffer[MAX_LINE_LENGTH];

    printf("%s", WELCOME_MESSAGE);

    while (1) {
        shellPrompt();
        readLine(buffer, MAX_LINE_LENGTH);

        size_t len = strlen(buffer);
        if (len == 0) continue;

        if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';

        if (CommandParse(buffer) == ERROR) {
            printf("%s", NOT_FOUND_MESSAGE);
        }
    }
}

void readLine(char *buf, int maxLen) {
    int len = 0;
    char c;

    while (1) {
        c = getchar();
        if (c == '\n') {
            putchar('\n');
            break;
        }
        if (c == '\b') {
            if (len > 0) {
                len--;
                putchar('\b'); putchar(' '); putchar('\b');
            }
            else{
                sys_beep(500, 100);
            }
        } else if ((unsigned char)c == 0x12) { // CTRL+R especial
            takeRegistersSnapshot();
            printf("\n[Snapshot de registros tomado]\n");
            shellPrompt(); // Añadir el prompt después del mensaje
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