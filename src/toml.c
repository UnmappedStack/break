#include <string.h>
#include <toml.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static bool is_lower_char(char c) {
    return c >= 'a' && c <= 'z';
}

// Caller must free the `name` field.
ConfigFile parse_toml(char *contents) {
    ConfigFile cfg = {0};
    bool skip_line = false;
    while (*contents) {
        if (skip_line) {
            if (*contents == '\n')
                skip_line = false;
        } else if (*contents == '#' || *contents == '[' || *contents == ' ' || *contents == '\n')
            skip_line = true;
        else if (is_lower_char(*contents)) {
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
            if (!strcmp(option, "name")) {
                cfg.project_name = (char*) malloc(strlen(val) + 1);
                strcpy(cfg.project_name, val);
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
        } else {
            invalid_char:
            printf("Invalid character \"%c\" in config toml file.\n", *contents);
            exit(1);
        }
        contents++;
    }
    return cfg;
}
