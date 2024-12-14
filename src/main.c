#include <stdio.h>

void help(char *path) {
    printf("Usage:\n"
           "%s build   -> builds the program and outputs it to ./target/debug/program_name\n"
           "%s run     -> builds the program and runs it\n"
           "%s runonly -> runs the program without rebuilding it\n"
           "%s version -> display version information\n"
           "%s help    -> shows this help information\n\n"
           "Note that configuration of the build system itself is done in break.toml.\n",
            path, path, path, path, path);
}

void run_command(char *cmd) {
    printf("TODO: Implement run_command\n");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Not enough arguments.\n");
        help(argv[0]);
        return 1;
    } else if (argc == 2)
        run_command(argv[1]);
    else {
        printf("Too many arguments. Try running:\n%s help\n", argv[0]);
        return 1;
    }
}
