#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

char* getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    printf("Last modified time: %s", ctime(&attr.st_mtime));
    printf("");
    return(ctime(&attr.st_mtime));
}

