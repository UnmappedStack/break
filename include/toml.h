#pragma once
#include <stdbool.h>

typedef struct {
    char *project_name;
    char compiler[6];
    char version[9];
    char edition[4];
    bool freestanding;
} ConfigFile;

ConfigFile parse_toml(char *contents);
