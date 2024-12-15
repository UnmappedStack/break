#include <os.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
void makedir(char *path) {
    int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0 && errno != EEXIST) {
        int err = errno;
        printf("Failed to create new directory with errno = %i\n", err);
        exit(1);
    }
}

bool is_dir_empty(char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR &&
           (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))) {
            continue;
        }
        return true;
    }
    return false;
}

// Caller must free afterward
char *get_current_dir() {
    char *cwd_full = (char*) malloc(PATH_MAX);
    getcwd(cwd_full, PATH_MAX);
    return cwd_full;
}

void change_current_dir(char *path) {
    if (chdir(path) < 0) {
        int err = errno;
        printf("Failed to enter directory \"%s\" with errno = %i\n", path, err);
        exit(1);
    }
}

Directory *open_dir(char *path) {
    return opendir(path);
}

// Caller must free
DirEntry *dir_iter(Directory *dir) {
    struct dirent *entry = readdir(dir);
    if (!entry) return NULL;
    uint32_t type = DIR_TYPE_UNK;
    if (entry->d_type == DT_DIR)
        type = DIR_TYPE_DIR;
    else if (entry->d_type == DT_REG)
        type = DIR_TYPE_REG;
    DirEntry *ent_new = (DirEntry*) malloc(sizeof(DirEntry));
    *ent_new = (DirEntry) {
        .name = entry->d_name,
        .type = type,
    };
    return ent_new;
}

void cmd_spawn(Command *cmd) {
    pid_t pid = fork();
    if (!pid) {
        execvp(cmd->argv[0], cmd->argv);
        exit(EXIT_FAILURE); // Failed to exec
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)  {
            printf("Failed to execute \"%s\"\n", cmd->argv[0]);
            exit(1);
        } else if (WIFSIGNALED(status) || (WIFEXITED(status) && WEXITSTATUS(status))) exit(1);
        for (size_t arg = 0; arg < cmd->argc; arg++)
            free(cmd->argv[arg]);
        free(cmd->argv);
    }
}
#else
#error "Sorry, currently only Linux is supported for break."
#endif
