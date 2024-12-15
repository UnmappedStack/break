#include <os.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__
#include <errno.h>
#include <sys/stat.h>
void makedir(char *path) {
    int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0 && errno != EEXIST) {
        int err = errno;
        printf("Failed to create new directory with errno = %i\n", err);
        exit(1);
    }
}
#else
#error "Sorry, currently only Linux is supported for break."
#endif
