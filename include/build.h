#pragma once
#include <toml.h>

int build_project(char **args, ConfigFile *cfg_ret);
int read_toml(ConfigFile *cfg);
