#include <os.h>
#include <string.h>
#include <build.h>
#include <toml.h>
#include <stdlib.h>
#include <stdio.h>

// Caller must do free on project_name ConfigFile entry
int read_toml(ConfigFile *cfg) {
    FILE *f = fopen("break.toml", "r");
    if (!f) {
        printf("Couldn't find break.toml, it doesn't exist.\n");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = (char*) malloc(fsize);
    fread(buf, fsize, 1, f);
    *cfg = parse_toml(buf);
    free(buf);
    fclose(f);
    return 0;
}

void compile_file(ConfigFile *cfg, char *filename, char **linker_list) {
    size_t filename_len = strlen(filename);
    char *obj_filename = (char*) malloc(filename_len + 3);
    strcpy(obj_filename, filename);
    memcpy(obj_filename, "obj", 3);
    strcpy(obj_filename + filename_len, ".o");
    Command cmd = cmd_new(cfg->compiler);
    char *args[] = {"-g",
                    "-Iinclude",
                    "-c", filename,
                    "-o", obj_filename,
                    NULL};
    cmd_args(&cmd, args);
    if (cfg->release)
        cmd_arg(&cmd, "-O2");
    if (cfg->warnerror) {
        char *errargs[] = {"-Wall", "-Werror", NULL};
        cmd_args(&cmd, errargs);
    }
    if (cfg->ccflags) {
        cmd_args_str(&cmd, cfg->ccflags);
    }
    cmd_print(&cmd);
    cmd_spawn(&cmd);
    *linker_list = realloc(*linker_list, strlen(*linker_list) + strlen(obj_filename) + 3);
    sprintf(*linker_list, "%s%s ", *linker_list, obj_filename);
    free(obj_filename);
}

void compile_dir(ConfigFile *cfg, char *dirname, char **linker_list) {
    Directory *dir = open_dir(dirname);
    DirEntry *entry;
    while ((entry = dir_iter(dir)) != NULL) {
        if (entry->type == DIR_TYPE_DIR) {
            if (!strcmp(entry->name, ".") || !strcmp(entry->name, "..")) continue;
            size_t fmtlen = strlen(dirname) + strlen(entry->name) + 2;
            char *full_dir = (char*) malloc(fmtlen);
            sprintf(full_dir, "%s/%s", dirname, entry->name);
            char *full_dir_obj = (char*) malloc(fmtlen);
            strcpy(full_dir_obj, full_dir);
            memcpy(full_dir_obj, "obj", 3);
            makedir(full_dir_obj);
            printf(" -> Enter directory: %s\n", full_dir);
            compile_dir(cfg, full_dir, linker_list);
            printf(" -> Exit directory: %s\n", full_dir);
            free(full_dir_obj);
            free(full_dir);
        } else if (entry->type == DIR_TYPE_REG) {
            size_t fmtlen = strlen(dirname) + strlen(entry->name) + 2;
            char *full_filename = (char*) malloc(fmtlen);
            sprintf(full_filename, "%s/%s", dirname, entry->name);
            compile_file(cfg, full_filename, linker_list);
            free(full_filename);
        }
        free(entry);
    }
}

// If cfg_ret is a valid address, it'll write the cfg to it, but with project_name freed.
// If it's NULL, it won't write anything.
int build_project(char **args, ConfigFile *cfg_ret) {
    ConfigFile cfg;
    if (read_toml(&cfg)) {
        printf("Failed to parse toml\n");
        return 1;
    }
    while (*args) {
        if (!strcmp(*args, "--release"))
            cfg.release = true;
        if (!strcmp(*args, "--install"))
            cfg.install = true;
        args++;
    }
    if (!cfg.compiler[0]) {
        printf("No compiler specified in break.toml! Could not build.");
        return 1;
    }
    char *output_dir = (cfg.release) ? "release" : "debug";
    makedir("target");
    makedir("target/release");
    makedir("target/debug");
    printf(" -> Cleaning object files...\n");
    // It takes a sacrifice of an arm and a half to delete a directory
    // so for simplicity (and shitty code), just use `rm -rf`.
    FILE *obj_dir = fopen("obj", "r");
    if (obj_dir) {
        // TODO: Replace with a proper function for deleting a directory
        Command cmd = cmd_new(      "rm" );
                      cmd_arg(&cmd, "-rf");
                      cmd_arg(&cmd, "obj");
        cmd_print(&cmd);
        cmd_spawn(&cmd);
        fclose(obj_dir);
    }
    makedir("obj");
    char *linker_list = (char*) malloc(1);
    linker_list[0] = 0;
    compile_dir(&cfg, "src", &linker_list);
    size_t proj_name_len = strlen(cfg.project_name);
    size_t dest_size  = strlen("target//") + strlen(output_dir) + proj_name_len + 1;
    char  *dest = (char*) malloc(dest_size);
    snprintf(dest, dest_size, "target/%s/%s", output_dir, cfg.project_name);
    Command ld_cmd = cmd_new(cfg.compiler);
    cmd_args_str(&ld_cmd, linker_list);
    char *ld_cmd_args[] = {"-o", dest, cfg.packages, NULL};
    cmd_args(&ld_cmd, ld_cmd_args);
    if (cfg.freestanding) {
        char *freestanding_args[] = {"-ffreestanding", "-nostdlib", NULL};
        cmd_args(&ld_cmd, freestanding_args);
    }
    if (cfg.ldflags)
        cmd_args_str(&ld_cmd, cfg.ldflags);
    cmd_print(&ld_cmd);
    cmd_spawn(&ld_cmd);
    if (cfg.install) {
        Command cp_cmd = cmd_new(         "sudo"    );
                         cmd_arg(&cp_cmd, "cp"      );
                         cmd_arg(&cp_cmd, dest      );
                         cmd_arg(&cp_cmd, "/usr/bin");
        cmd_print(&cp_cmd);
        cmd_spawn(&cp_cmd);
    }
    free(dest);
    free(cfg.project_name);
    free(cfg.packages);
    if (cfg.ccflags) free(cfg.ccflags);
    if (cfg.ldflags) free(cfg.ldflags);
    if (cfg_ret)
        *cfg_ret = cfg;
    return 0;
}
