#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void printdir(char *path) {
  DIR *dp = opendir(path);
  struct dirent *d;
  // Imprimo el path principal
  printf("%s\n", path);
  while ((d = readdir(dp))) {
    const char *name = d->d_name;
    char path_dir[1024];
    // Se imprime nombre del directorio
    printf("%s\n", name);

    // Concatenar path con nombre de archivo.
    strcpy(path_dir, path);
    strcat(path_dir, "/");
    strcat(path_dir, name);

    // Si es un directorio itero recursivamente.
    struct stat file_info;
    stat(path_dir, &file_info);
    if (S_ISDIR(file_info.st_mode) && (name[0] != '.')) {
      // LLamada recursiva.
      printdir(path_dir);
    }
  }
  closedir(dp);
}
int main(int argc, char *argv[]) {
  printdir(argv[1]);
  return 0;
}
