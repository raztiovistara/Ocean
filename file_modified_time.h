#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

std::string getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    std::cout<<"Last modified time: "<<(long)attr.st_mtime<<std::endl;
    //printf("Last modified time: %ld\n", (long)attr.st_mtime);
    return(std::to_string(attr.st_mtime));
}

