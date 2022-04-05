#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
	int fd = open("foo", O_CREAT | O_WRONLY | O_TRUNC | S_IRUSR, S_IWUSR);
	void *buffer = NULL;
	int rc = read(fd, buffer, 1);
	printf("Read: %d\n", rc);
	printf("FD value: %d\n", fd);
	return 0;
}