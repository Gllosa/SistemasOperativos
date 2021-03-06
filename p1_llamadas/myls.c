#include <stdio.h>		/* Header file for system call printf */
#include <unistd.h>		/* Header file for system call gtcwd */
#include <sys/types.h>		/* Header file for system calls opendir, readdir y closedir */
#include <dirent.h>
#include <string.h>


int main(int argc, char *argv[])
{	
    struct dirent *lectura;
    DIR *directorio;

    char path[PATH_MAX];

    if (argc < 1) {
        printf("Not enough arguments\n");
        return -1;
    }

    if (argc > 1){
        directorio = opendir(argv[1]);
    }

    if (argc == 1){
        if (getcwd(path, PATH_MAX) != NULL){
            directorio = opendir(path);
        }else{
            perror("Unable to get current path");
            return -1;
        }
    }

    if (directorio == NULL) {
        printf ("Cannot open directory");
        return -1;
    }

    while ((lectura = readdir(directorio)) != NULL) {
        printf ("%s\n", lectura->d_name);
    }

    closedir (directorio);
    return 0;
}

