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
    if (fstat(fileno(f), &lenbuf) < 0) {
        printf("fstat failed.\n");
        fclose(f);
        return 1;
    }
    fseek(f, 0, SEEK_SET);
    char *buf = (char*) malloc(lenbuf.st_size);
    fread(buf, lenbuf.st_size, 1, f);
    *cfg = parse_toml(buf);
    free(buf);
    fclose(f);
    return 0;
}

void compile_file(ConfigFile *cfg, char *filename, char **linker_list) {
    char opt_level = (cfg->release) ? '2' : '0';
    size_t filename_len = strlen(filename);
    size_t fmtlen = strlen(cfg->compiler) + strlen(" -I include -c ") + filename_len * 2 + strlen(" -o ") + 5;
    if (cfg->warnerror)
        fmtlen += strlen(" -Wall -Werror");
    if (cfg->ccflags)
        fmtlen += strlen(cfg->ccflags) + 2;
    char *buf = (char*) malloc(fmtlen);
    char *obj_filename = (char*) malloc(filename_len + 3);
    strcpy(obj_filename, filename);
    memcpy(obj_filename, "obj", 3);
    memcpy(obj_filename + filename_len, ".o\0", 3);
    char *err_options = (cfg->warnerror) ? " -Wall -Werror" : "";
    char *ccflags = (cfg->ccflags) ? cfg->ccflags : "";
    sprintf(buf, "%s -I include -c %s -o %s -O%c%s%s", cfg->compiler, filename, obj_filename, opt_level, err_options, ccflags);
    printf(" -> %s\n", buf);
    // TODO: Stop recalculating string lengths
    *linker_list = realloc(*linker_list, strlen(*linker_list) + strlen(obj_filename) + 3);
    sprintf(*linker_list, "%s%s ", *linker_list, obj_filename);
    system(buf);
    free(obj_filename);
    free(buf);
}

void compile_dir(ConfigFile *cfg, char *dirname, char **linker_list) {
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
            size_t fmtlen = strlen(dirname) + strlen(entry->d_name) + 2;
            char *full_dir = (char*) malloc(fmtlen);
            sprintf(full_dir, "%s/%s", dirname, entry->d_name);
            char *full_dir_obj = (char*) malloc(fmtlen);
            strcpy(full_dir_obj, full_dir);
            memcpy(full_dir_obj, "obj", 3);
            mkdir(full_dir_obj, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            printf(" -> Enter directory: %s\n", full_dir);
            compile_dir(cfg, full_dir, linker_list);
            printf(" -> Exit directory: %s\n", full_dir);
            free(full_dir_obj);
            free(full_dir);
        } else if (entry->d_type == DT_REG) {
            size_t fmtlen = strlen(dirname) + strlen(entry->d_name) + 2;
            char *full_filename = (char*) malloc(fmtlen);
            sprintf(full_filename, "%s/%s", dirname, entry->d_name);
            compile_file(cfg, full_filename, linker_list);
            free(full_filename);
        }
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
    mkdir("target", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("target/release", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("target/debug", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    printf(" -> Cleaning object files...\n");
    // It takes a sacrifice of an arm and a half to delete a directory
    // so for simplicity (and shitty code), just use the system() with `rm -rf`.
    struct stat info;
    stat("obj", &info);
    if (info.st_mode & S_IFDIR)
        system("rm -rf obj");
    mkdir("obj", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    char *linker_list = (char*) malloc(1);
    linker_list[0] = 0;
    compile_dir(&cfg, "src", &linker_list);
    size_t link_cmd_len = strlen(cfg.packages) + strlen(cfg.compiler) + strlen(" -o target// ") + strlen(output_dir) + strlen(cfg.project_name) + strlen(linker_list) + 3;
    if (cfg.freestanding)
        link_cmd_len += strlen("-ffreestanding -nostdlib");
    if (cfg.ldflags)
        link_cmd_len += strlen(cfg.ldflags) + 1;
    char *link_cmd = (char*) malloc(link_cmd_len);
    char *freestanding_flags = (cfg.freestanding) ? "-ffreestanding -nostdlib" : "";
    char *ldflags = (cfg.ldflags) ? cfg.ldflags : "";
    snprintf(link_cmd, link_cmd_len, "%s -o target/%s/%s %s%s%s%s", cfg.compiler, output_dir, cfg.project_name, linker_list, freestanding_flags, cfg.packages, ldflags);
    printf(" -> %s\n", link_cmd);
    system(link_cmd);
    if (cfg.install) {
        size_t proj_name_len = strlen(cfg.project_name);
        size_t src_size  = strlen("target//") + strlen(output_dir) + proj_name_len + 1;
        size_t total_cmd_size = strlen("sudo cp ") + src_size + 1 + sizeof("/usr/bin");
        char *cp_cmd = (char*) malloc(total_cmd_size);
        snprintf(cp_cmd, total_cmd_size, "sudo cp target/%s/%s /usr/bin", output_dir, cfg.project_name);
        printf(" -> %s\n", cp_cmd);
        system(cp_cmd);
    }
    free(link_cmd);
    free(cfg.project_name);
    free(cfg.packages);
    if (cfg.ccflags) free(cfg.ccflags);
    if (cfg.ldflags) free(cfg.ldflags);
    if (cfg_ret)
        *cfg_ret = cfg;
    return 0;
}
