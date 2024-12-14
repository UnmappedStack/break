#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdio.h>
#include <build.h>
#include <run.h>
#include <toml.h>
#include <string.h>

int run_project(char **argv) {
    ConfigFile cfg;
    if (read_toml(&cfg)) return 1;
    char *path = (char*) malloc(strlen("./target/") + strlen(cfg.project_name) + 1);
    sprintf(path, "./target/%s", cfg.project_name);
    size_t argc = 0;
    while (argv[argc]) argc++;
    char **new_argv = (char**) malloc(argc + 2);
    new_argv[0] = path;
    printf(" -> %s ", path);
    for (size_t i = 1; argv[i]; i++) {
        new_argv[i] = argv[i - 1];
        printf("%s ", new_argv[i]);
    }
    printf("\n");
    execvp(path, new_argv); 
    printf("Failed to run program, errno = %i\n", errno);
    return 0;
}

int build_and_run(char **argv) {
    if (build_project()) return 1;
    return run_project(argv);
}
