#include <stdio.h>	/*Header file for system call printf*/
#include <sys/types.h>	/*Header file for system call fstat*/
#include <sys/stat.h>
#include <fcntl.h>	/*Header file for system call fcntl*/
#include <dirent.h> 	/*Header file for system call opendir, closedir,readdir...*/
#include <unistd.h>


int main(int argc, char *argv[])
{
	int fd;

	struct dirent *lectura;
	DIR *directorio;

	char path[PATH_MAX];

	if (getcwd(path, PATH_MAX) == NULL){
		perror("Unable to get current path");
		return -1;
	}

    directorio = opendir(path);

	while ((lectura = readdir(directorio)) != NULL) {
        if (lectura -> d_type == DT_REG){
			if ((fd = open(lectura -> d_name, O_RDONLY)) < 0){
				perror("File could not be opened\n");
				close(fd);
			}
			int posicion = lseek(fd, 0, SEEK_END);
			printf("%s	%i\n", lectura -> d_name, posicion);
			close(fd);
		}
	}
	closedir(directorio);

	return 0;
}
