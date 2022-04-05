#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>

/*

Se pretende mostrar que tanto el padre como el hijo 
comparten el mismo OPEN FILE TABLE Por eso cuando
el hijo cambia la posición con lseek también le
al proceso padre.

*/

int main(int argc, char *argv[]){
    //int fd = open("file.txt", O_RDONLY);
    int fd = open("file.txt", O_RDWR|O_CREAT, 0666);
    assert(fd >= 0);
    int rc = fork();
    if (rc == 0){
        // rc devuelve el offset
        // SEEK_SET es offset desde la pos 0.
        rc = lseek(fd, 10, SEEK_SET);
        printf("Child offset: %d\n", rc);
    }else if(rc > 0){
        wait(NULL); // Espera que termine el hijo
        printf("Parent offset: %d\n", (int)lseek(fd, 0, SEEK_CUR)); // Imprime la posición actual.
    }
    return 0;
}