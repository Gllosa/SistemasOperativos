#include <stdio.h>		/* Header file for printf */
#include <sys/types.h>		/* Header file for system call  open */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>		/* Header file for system calls read, write y close */

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	int fd, n, ch;

	/*If the number of arguments is less than two (argv[0] -> program, argv[|] -> file to be shown) we print the error and return -1*/
	if(argc < 2)
	{
		printf("Not enough arguments\n");
		return -1;
	}
	
	/*Intentamos abrir el archivo que recibimos en argv[1], si este no existe imprimimos un error*/
	if ((fd=open(argv[1], O_RDONLY))<0) {
		perror("File does not exist\n");
		exit(1);
  }
	while ((n=read(fd, &ch, BUFFER_SIZE))>0){
		write(STDOUT_FILENO, &ch, BUFFER_SIZE);
		close(fd);
	}


	

	return 0;
}

