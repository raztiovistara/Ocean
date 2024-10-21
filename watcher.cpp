#include <iostream>
#include <sys/stat.h>
#include <ctime>
#include "file_modified_time.h"

int main() {
    getFileCreationTime("G://ToDo.txt");
    return 0;
}
