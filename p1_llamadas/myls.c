#include <stdio.h>		/* Header file for system call printf */
#include <unistd.h>		/* Header file for system call gtcwd */
#include <sys/types.h>		/* Header file for system calls opendir, readdir y closedir */
#include <dirent.h>
#include <string.h>


int main(int argc, char *argv[])
{	
		struct dirent *lectura;
        DIR *directorio;

        // Ensure correct argument count.

        if (argc != 2) {
            printf ("Usage: testprog <dirname>\n");
            return 1;
        }

        // Ensure we can open directory.

        directorio = opendir (argv[1]);
        if (directorio == NULL) {
            printf ("Cannot open directory '%s'\n", argv[1]);
            return 1;
        }

        // Process each entry.

        while ((lectura = readdir(directorio)) != NULL) {
            printf ("%s\n", lectura->d_name);
        }

        // Close directory and exit.

        closedir (directorio);
        return 0;
    }

