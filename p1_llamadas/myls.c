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

    // Si tenemos mas de un argumento es que nos ha llegado una ruta
    if (argc > 1){
        directorio = opendir(argv[1]);
    }

    // Si tenemos un solo argumento significa que no nos ha llegado una ruta y por lo tanto obtenemos la ruta actual
    if (argc == 1){
        if (getcwd(path, PATH_MAX) != NULL){
            directorio = opendir(path);
        }else{
            perror("Unable to get current path");
            return -1;
        }
    }

    // Devolvemos un error si no hemos podido abrir el directorio
    if (directorio == NULL) {
        printf ("Cannot open directory");
        return -1;
    }

    // Leemos las entradas del directorio y las imprimimos
    while ((lectura = readdir(directorio)) != NULL) {
        printf ("%s\n", lectura->d_name);
    }

    //cerramos el directorio
    closedir (directorio);
    return 0;
}

