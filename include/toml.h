#pragma once

typedef struct {
    char *project_name;
    char compiler[6];
    char version[9];
    char edition[4];
} ConfigFile;

ConfigFile parse_toml(char *contents);
