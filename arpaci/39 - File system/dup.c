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

Con dup podemos crear un nuevo file descriptor que
se refiere al existene.

*/

int main(int argc, char *argv[]){
    int fd = open("file.txt", O_RDWR|O_CREAT, 0666);

    int fd2 = dup(fd);
    printf("File descriptor 1: %d\n", fd);
    printf("File descriptor 2: %d\n", fd2);

    write(fd, "Hola\n", 5);
    write(fd2, "hola2\n", 6);

    close(fd);

    return 0;
}