#include <run.h>
#include <build.h>
#include <toml.h>
#include <init.h>
#include <string.h>
#include <stdio.h>

void help(char *path) {
    const char *commands[] = {
            "%s init               -> creates a new break project in the current directory.\n",
            "%s new [projectname]  -> creates a new break project in a new directory.\n",
            "%s build              -> builds the program and outputs it to ./target/debug/program_name\n",
            "%s run                -> builds the program and runs it\n",
            "%s runonly            -> runs the program without rebuilding it\n",
            "%s version            -> display version information\n",
            "%s help               -> shows this help information\n\n",
    };
    printf("Usage:\n");
    size_t num_commands = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < num_commands; i++) printf(commands[i], path);
    printf("Note that configuration of the build system itself is done in break.toml.\n");
}

void version() {
    printf("break 0.0.1\n"
           "Copyright (C) 2024 Jake Steinburger (UnmappedStack) under the Mozilla Public License 2.0\n"
           "See the GitHub repository for more information.\n");
}

int run_command(char *path, char *cmd, char **args) {
    if (!strcmp(cmd, "build"))
        return build_project(args, NULL);
    else if (!strcmp(cmd, "init"))
        init_project();
    else if (!strcmp(cmd, "new"))
        new_project(args);
    else if (!strcmp(cmd, "run"))
        build_and_run(args);
    else if (!strcmp(cmd, "runonly"))
        return run_project(args, 2);
    else if (!strcmp(cmd, "version"))
        version();
    else if (!strcmp(cmd, "help"))
        help(path);
    else {
        printf("Invalid command: %s\n"
               "Try running:\n"
               "%s help\n", cmd, path);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Not enough arguments.\n");
        help(argv[0]);
        return 1;
    } else
        return run_command(argv[0], argv[1], &argv[2]);
    return 0;
}
