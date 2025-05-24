#ifndef COMMANDS_H
#define COMMANDS_H

#define REGISTERS_CANT 17


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
void regsCmd(void);
#endif
