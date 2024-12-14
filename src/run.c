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

// If is_release is 1, then it only tries to run it as release.
// If it's 0, then it only tries to run it as debug.
// If it's 2, then it first tries to run it as release, and if it fails then it runs it as debug.
int run_project(char **argv, int is_release) {
    ConfigFile cfg;
    if (read_toml(&cfg)) return 1;
    char *exec_dir = (is_release) ? "release" : "debug";
    char *path = (char*) malloc(strlen("./target/") + strlen(exec_dir) + strlen(cfg.project_name) + 2);
    sprintf(path, "./target/%s/%s", exec_dir, cfg.project_name);
    size_t argc = 0;
    while (argv[argc]) argc++;
    if (argc == 0) argc = 1;
    char **new_argv = (char**) malloc(argc + 2);
    new_argv[0] = path;
    printf(" -> %s ", path);
    // Fuck me, I'm addicted to gotos. This is unhealthy, broken, and horrible.
    // but, I will do anything to have less indentation.
    if (!argv[0]) goto end_of_loop;
    for (size_t i = 1; argv[i]; i++) {
        new_argv[i] = argv[i - 1];
        printf("%s ", new_argv[i]);
    }
    end_of_loop:
    printf("\n");
    execvp(path, new_argv); 
    if (is_release == 2 && !strcmp(exec_dir, "release"))
        return run_project(argv, 0);
    return 0;
}

int build_and_run(char **argv) {
    ConfigFile cfg;
    if (build_project(argv, &cfg)) return 1;
    return run_project(argv, (int) cfg.release);
}
