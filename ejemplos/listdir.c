#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int isdir(char const *path) {
  struct stat file_info;
  stat(path, &file_info);
  return S_ISDIR(file_info.st_mode);
}
void printcont(char const *path) {
  DIR *d;
  struct dirent *dir;
  // Imprimo path
  printf("%s\n", path);
  // Imprimo contenido
  d = opendir(path);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if ((dir->d_name)[0] != '.')
        printf("%s\n", dir->d_name);
    }
  }
  closedir(d);
}
void printdir(char *path) {
  DIR *d;
  char path_dir[1024];
  struct dirent *dir;
  strcat(path, "/");
  printcont(path);
  // Recorro directorios
  d = opendir(path);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      strcpy(path_dir, path);
      strcat(path_dir, dir->d_name);
      // Si es un directorio, recorro recursicamente.
      if (((dir->d_name)[0] != '.') && isdir(path_dir)) {
        printdir(path_dir);
      }
    }
  }
  closedir(d);
}
int main(int argc, char *argv[]) {
  printdir(argv[1]);
  return 0;
}
