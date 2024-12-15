#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <os.h>

Command cmd_new(char *program) {
    Command cmd = {0};
    cmd.argv = (char**) malloc(sizeof(char**) * 2);
    cmd.argv[0] = (char*) malloc(strlen(program) + 1);
    cmd.argv[1] = NULL;
    strcpy(cmd.argv[0], program);
    cmd.argv_size = 16;
    cmd.argc = 1;
    return cmd;
}

void cmd_arg(Command *cmd, char *arg) {
    cmd->argv_size += 8;
    cmd->argv = realloc(cmd->argv, cmd->argv_size);
    cmd->argv[cmd->argc] = (char*) malloc(strlen(arg) + 1);
    strcpy(cmd->argv[cmd->argc], arg);
    cmd->argv[++cmd->argc] = NULL;
}

void cmd_args(Command *cmd, char **args) {
    while (*args)
        cmd_arg(cmd, *(args++));
}

void cmd_args_str(Command *cmd, char *args) {
    if (*args == ' ') args++;
    size_t start = 0;
    size_t i = 0;
    while (args[i]) {
        if (args[i] == ' ') {
            // handle spaces
            args[i] = 0;
            cmd_arg(cmd, &args[start]);
            start = i + 1;
        }
        i++;
    }
}

void cmd_print(Command *cmd) {
    printf(" -> ");
    for (size_t i = 0; i < cmd->argc; i++)
        printf("%s ", cmd->argv[i]);
    printf("\n");
}
