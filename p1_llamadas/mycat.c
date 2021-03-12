#include <stdio.h>		/* Header file for printf */
#include <sys/types.h>		/* Header file for system call  open */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>		/* Header file for system calls read, write y close */

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	int fd, n_leidos;
	char buffer[BUFFER_SIZE];

	// If the number of arguments is less than two (argv[0] -> program, argv[|] -> file to be shown) we print the error and return -1
	if(argc < 2)
	{
		perror("No hay suficientes argumentos\n");
		return -1;
	}
	
	// Intentamos abrir el archivo que recibimos en argv[1], si este no existe imprimimos un error
	int i;
	for (i = 0; i < (argc - 1); i++){
		if ((fd=open(argv[1 + i], O_RDONLY)) < 0) {
			perror("No existe el archivo o directorio\n");
			close(fd);
			return -1;
	}
	// Bucle lectura del fichero
		while ((n_leidos = read(fd, buffer, BUFFER_SIZE)) > 0){
			write(STDOUT_FILENO, buffer, n_leidos);	
		}

		close(fd);}
	return 0;
}

