#include <stdio.h>		/* Header file for system call printf */
#include <unistd.h>		/* Header file for system call gtcwd */
#include <sys/types.h>		/* Header file for system calls opendir, readdir y closedir */
#include <dirent.h>
#include <string.h>


int main(int argc, char *argv[])
{	
    // Tenemos un array de directorios, ya que no se puede llegar mas de uno
    DIR *directorios[argc - 1];
    DIR *directorio;
    struct dirent *lectura;
    
    char path[PATH_MAX];

    // Si tenemos mas de un argumento es que nos ha llegado una o más rutas
    int i;

    if (argc > 1){
        for (i = 0; i < (argc - 1); i++){
            // Abrimos los directorios y los metemos en el array
            directorios[i] = opendir(argv[i + 1]);
        }
    }
    // Si tenemos un solo argumento significa que no nos ha llegado una ruta y por lo tanto obtenemos la ruta actual
    if (argc == 1){
        if (getcwd(path, PATH_MAX) != NULL){
            directorio = opendir(path);
        }else{
            perror("No es posible obtener la ruta actual");
            return -1;
        }
    }

    // Devolvemos un error si no hemos podido abrir alguno de los directorios
    // Si tenemos una lista de directorios iteramos sobre ellos y mostramos sus contenidos
    // Si no simplemente mostramos el del directorio recuperado con el getpwd().
    if (sizeof(directorios) > 1){
        for (i = 0; i < (argc - 1); i++){
            if (directorios[i] == NULL) {
                printf ("Cannot open directory");
                return -1;
            }
            // Leemos las entradas del directorio y las imprimimos
            while ((lectura = readdir(directorios[i])) != NULL) {
                printf ("%s\n", lectura->d_name);
            }
            // Cerramos el directorio
            closedir(directorios[i]);
        }
        
    }else{
        while ((lectura = readdir(directorio)) != NULL) {
            printf ("%s\n", lectura->d_name);
        }
    
    }
    
    return 0;

}