#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>

/*

Con stat obtenemos metada

*/

int main(int argc, char *argv[]){
    struct stat file_info;
    stat(argv[1], &file_info);
    printf("Argument: %s\n", argv[1]);

    printf("File user size: %d\n", (int)file_info.st_size);
    printf("File mode: %d\n", S_ISDIR(file_info.st_mode));
    return 0;
}