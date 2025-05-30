#ifndef COMMANDS_H
#define COMMANDS_H

#define REGISTERS_CANT 17
#define MAX_ARGS 3

#define OK 0
#define ERROR -1
#define EXIT_CODE 1

typedef int (*cmd_fn)(int argc, char *argv[]);


typedef struct{
    const char *name;
    cmd_fn function;
    const char *help;
}TShellCmd;

extern const TShellCmd shellCmds[];

int helpCmd(int argc, char *argv[]);
int exitCmd(int argc, char *argv[]);
int setUserCmd(int argc, char *argv[]);
int clearCmd(int argc, char *argv[]);
int timeCmd(int argc, char *argv[]);
int fontSizeCmd(int argc, char *argv[]);
int regsCmd(int argc, char *argv[]);
int fillCommandAndArgs(char *args[], char *input);  // declaración de la función
int CommandParse(char *commandInput);
int exceptionCmd(int argc, char *argv[]);


#endif
