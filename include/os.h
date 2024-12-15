#pragma once
#include <stddef.h>

typedef struct {
    size_t argv_size; // NOT argc! It's the size of argv *in bytes*.
    int argc;
    char **argv; // cmd name is always first arg
} Command;

Command cmd_new(char *program);
void cmd_arg(Command *cmd, char *arg);
void cmd_args(Command *cmd, char *args[]);
void cmd_args_str(Command *cmd, char *args);
void cmd_print(Command *cmd);
void cmd_spawn(Command *cmd);

void makedir(char *path);
