#include <string.h>
#include <toml.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

static bool is_lower_char(char c) {
    return c >= 'a' && c <= 'z';
}

void validate_dep(char *dep) {
    printf("Validating dependency %s\n", dep);
    if (dep[0] != 'l') {
        printf("Invalid dependency, libraries always start with the letter l.\n");
        exit(1);
    }
    size_t dep_len = strlen(dep);
    char *expanded = (char*) malloc(dep_len + 3);
    memcpy(expanded + 3, dep + 1, dep_len - 1);
    memcpy(expanded, "lib", 3);
    size_t path_len = strlen(expanded) + strlen("/usr/lib/.so") + 1;
    char *path = (char*) malloc(path_len);
    snprintf(path, path_len, "/usr/lib/%s.so", expanded);
    if (!access(path, F_OK)) {
        free(expanded);
        free(path);
    } else {
        printf("Library \"%s\" doesn't exist (searched for library at %s).\n", dep, path);
        exit(1);
    }
}

// Caller must free the `name` field and the `packages` field
ConfigFile parse_toml(char *contents) {
    ConfigFile cfg = {0};
    bool skip_line = false;
    bool in_deps = false;
    cfg.packages = (char*) malloc(1);
    *cfg.packages = 0;
    while (*contents) {
        if (skip_line) {
            if (*contents == '\n')
                skip_line = false;
        } else if (*contents == '#' || *contents == ' ')
            skip_line = true;
        else if (*contents == '[') {
            if (!memcmp(contents, "[dependencies]", 14))
                in_deps = true;
            skip_line = true;
        } else if (is_lower_char(*contents)) {
            char option[15];
            size_t i = 0;
            while (is_lower_char(*contents) && i < 15) {
                option[i] = *contents;
                i++;
                contents++;
            }
            option[i] = 0;
            while (*contents != '"') {
                if (*contents != ' ' && *contents != '=')
                    goto invalid_char;
                contents++;
            }
            contents++;
            char *val = contents;
            while (*contents != '"') contents++;
            *contents = 0;
            skip_line = true;
            if (in_deps) {
                validate_dep(option);
                size_t new_deps_len = strlen(cfg.packages) + strlen(option) + 3;
                cfg.packages = realloc(cfg.packages, new_deps_len);
                snprintf(cfg.packages, new_deps_len, "%s -%s", cfg.packages, option);
            } else {
                if (!strcmp(option, "name")) {
                    cfg.project_name = (char*) malloc(strlen(val) + 1);
                    strcpy(cfg.project_name, val);
                } else if (!strcmp(option, "ccflags")) {
                    if (cfg.ccflags) {
                        printf("TODO: Allow ccflags to be specified multiple times. This feature is not yet implemented.\n");
                        exit(1);
                    }
                    size_t ccflags_len = strlen(val) + 2;
                    cfg.ccflags = (char*) malloc(ccflags_len);
                    snprintf(cfg.ccflags, ccflags_len, " %s", val);
                } else if (!strcmp(option, "ldflags")) {
                    if (cfg.ldflags) {
                        printf("TODO: Allow ldflags to be specified multiple times. This feature is not yet implemented.\n");
                        exit(1);
                    }
                    size_t ldflags_len = strlen(val) + 2;
                    cfg.ldflags = (char*) malloc(ldflags_len);
                    snprintf(cfg.ldflags, ldflags_len, " %s", val);
                } else if (!strcmp(option, "warnerror")) {
                    if (!strcmp(val, "true"))
                        cfg.warnerror = true;
                } else if (!strcmp(option, "freestanding")) {
                    if (!strcmp(val, "true"))
                        cfg.freestanding = true;
                } else if (!strcmp(option, "version"))
                    strcpy(cfg.version, val);
                else if (!strcmp(option, "edition"))
                    strcpy(cfg.edition, val);
                else if (!strcmp(option, "compiler"))
                    strcpy(cfg.compiler, val);
                else {
                    printf("Unrecognised option: \"%s\"\n", option);
                    exit(1);
                }
            }
        } else if (*contents != '\n') {
            invalid_char:
            printf("Invalid character \"%c\" in config toml file.\n", *contents);
            exit(1);
        }
        contents++;
    }
    return cfg;
}
