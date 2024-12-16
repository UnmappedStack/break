#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DIR_TYPE_DIR 0
#define DIR_TYPE_REG 1
#define DIR_TYPE_UNK 2

typedef struct {
    size_t argv_size; // NOT argc! It's the size of argv *in bytes*.
    int argc;
    char **argv; // cmd name is always first arg
} Command;

typedef struct {
    char    *name;
    uint32_t type;
} DirEntry;

#ifdef __linux__
#include <dirent.h>
typedef DIR Directory;
#endif

Command cmd_new(char *program);
void cmd_arg(Command *cmd, char *arg);
void cmd_args(Command *cmd, char *args[]);
void cmd_args_str(Command *cmd, char *args);
void cmd_print(Command *cmd);
void cmd_spawn(Command *cmd);

void makedir(char *path);
bool is_dir_empty(char *path);
char *get_current_dir();
void change_current_dir(char *path);
Directory *open_dir(char *path);
DirEntry *dir_iter(Directory *dir);
void rmrf(char *path);
