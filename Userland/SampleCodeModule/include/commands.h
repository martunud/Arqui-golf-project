#ifndef COMMANDS_H
#define COMMANDS_H

typedef void (*cmd_fn)(void);


typedef struct{
    const char *name;
    cmd_fn function;
    const char *help;
}TShellCmd;

extern const TShellCmd shellCmds[];

void helpCmd(void);
void exitCmd(void);
void setUserCmd(void);
void clearCmd(void);
void timeCmd(void);
void fontSizeCmd(void);
#endif
