#include <string.h>
#include <stdlib.h>
#include <init.h>
#include <stdio.h>
#include <os.h>

const char *configf_default = "[package]\n"
                              "name = \"%s\"\n"
                              "version = \"0.0.1\"\n"
                              "edition = \"2024\"\n"
                              "compiler = \"gcc\"\n";

const char *mainc_default = "#include <stdio.h>\n\n"
                            "int main() {\n"
                            "\tprintf(\"Hello world from break!\\n\");\n"
                            "}\n";

void init_project() {
    if (is_dir_empty(".")) {
        printf("Current directory is not empty, cannot initiate a new break project.\n");
        exit(1);
    }
    // Create break.toml
    char *cwd_full = get_current_dir();
    char *cwd = cwd_full + strlen(cwd_full) - 1;
    while (*cwd && cwd[-1] != '/') cwd--;
    FILE *config_file = fopen("break.toml", "w");
    if (!config_file) {
        printf("Failed to open break.toml.\n");
        free(cwd_full);
        exit(1);
    }
    fprintf(config_file, configf_default, cwd);
    fclose(config_file);
    // Create src/main.c
    makedir("src");
    FILE *mainc_file = fopen("src/main.c", "w");
    if (!mainc_file) {
        printf("Failed to open src/main.c.\n");
        exit(1);
    }
    fputs(mainc_default, mainc_file);
    fclose(mainc_file);
    printf("Successfully created new project in current directory called \"%s\".\n", cwd);
    free(cwd_full);
}

void new_project(char **argv) {
    if (!*argv) {
        printf("Project name not specified.\n");
        exit(1);
    }
    makedir(*argv);
    change_current_dir(*argv);
    init_project();
    change_current_dir("..");
}





