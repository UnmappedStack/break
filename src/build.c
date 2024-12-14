#include <dirent.h>
#include <string.h>
#include <build.h>
#include <toml.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>

// Caller must do free on project_name ConfigFile entry
int read_toml(ConfigFile *cfg) {
    FILE *f = fopen("break.toml", "r");
    if (!f) {
        printf("Couldn't find break.toml, it doesn't exist.\n");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    struct stat lenbuf;
    fstat(fileno(f), &lenbuf);
    fseek(f, 0, SEEK_SET);
    char *buf = (char*) malloc(lenbuf.st_size);
    fread(buf, lenbuf.st_size, 1, f);
    *cfg = parse_toml(buf);
    free(buf);
    fclose(f);
    return 0;
}

void compile_file(ConfigFile *cfg, char *filename) {
    (void) cfg;
    printf("Compiling \"%s\"\n", filename);
}

void compile_dir(ConfigFile *cfg, char *dirname) {
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
            size_t fmtlen = strlen(dirname) + strlen(entry->d_name) + 2;
            char *full_dir = (char*) malloc(fmtlen);
            sprintf(full_dir, "%s/%s", dirname, entry->d_name);
            printf("Enter directory: %s\n", full_dir);
            compile_dir(cfg, full_dir);
            printf("Exit directory: %s\n", full_dir);
            free(full_dir);
        } else if (entry->d_type == DT_REG) {
            compile_file(cfg, entry->d_name);
        }
    }
}

int build_project() {
    ConfigFile cfg;
    if (read_toml(&cfg)) {
        printf("Failed to parse toml\n");
        return 1;
    }
    if (!cfg.compiler[0]) {
        printf("No compiler specified in break.toml! Could not build.");
        return 1;
    }
    mkdir("target", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("target/obj", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    compile_dir(&cfg, "src");
    free(cfg.project_name);
    return 0;
}
