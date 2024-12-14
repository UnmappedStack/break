#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <init.h>
#include <stdio.h>
#include <dirent.h>
#include <linux/limits.h>
#include <sys/stat.h>

const char *configf_default = "[package]\n"
                              "name = \"%s\"\n"
                              "version = \"0.0.1\"\n"
                              "edition = \"2024\"\n";

const char *mainc_default = "#include <stdio.h>\n\n"
                            "int main() {\n"
                            "\tprintf(\"Hello world from break!\");\n"
                            "}\n";

void init_project() {
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR &&
           (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))) {
            continue;
        }
        printf("Current directory is not empty, cannot create new project.\n");
        exit(1);
    }
    // Create break.toml
    char *cwd_full = (char*) malloc(PATH_MAX);
    getcwd(cwd_full, PATH_MAX);
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
    mkdir("src", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
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
