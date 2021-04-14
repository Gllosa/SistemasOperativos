//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <wait.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/wait.h>

#include <signal.h>

#include <math.h>

#define MAX_COMMANDS 8

// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char * argv_execvp[8];

void siginthandler(int param) {
    printf("****  Saliendo del MSH **** \n");
    //signal(SIGINT, siginthandler);
    exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char ** * argvv, int num_command) {
    //reset first
    for (int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for (i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}

/**
 * Main sheell  Loop  
 */
int main(int argc, char * argv[]) {
    /**** Do not delete this code.****/
    int end = 0;
    int executed_cmd_lines = -1;
    char * cmd_line = NULL;
    char * cmd_lines[10];

    if (!isatty(STDIN_FILENO)) {
        cmd_line = (char * ) malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF) {
            if (strlen(cmd_line) <= 0) return 0;
            cmd_lines[end] = (char * ) malloc(strlen(cmd_line) + 1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush(stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ** * argvv = NULL;
    int num_commands;
    int total = 0;

    while (1) {

        error:;
        int status = 0;
        int command_counter = 0;
        int in_background = 0;
        signal(SIGINT, siginthandler);

        // Prompt 
        write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

        // Get command
        //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
        executed_cmd_lines++;
        if (end != 0 && executed_cmd_lines < end) {
            command_counter = read_command_correction( & argvv, filev, & in_background, cmd_lines[executed_cmd_lines]);
        } else if (end != 0 && executed_cmd_lines == end)
            return 0;
        else
            command_counter = read_command( & argvv, filev, & in_background); //NORMAL MODE
        //************************************************************************************************

        /************************ STUDENTS CODE ********************************/
        // Si el numero de comandos se pasa del máximo imprimimos un error
        if (command_counter > 0) {
            if (command_counter > MAX_COMMANDS)
                printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
        }

        // Obtenemos todos los comandos
        for (int cont = 0; cont < command_counter; cont++) {
            getCompleteCommand(argvv, cont);
        }

        // A partir de aquí definimos Mycalc

        // Comprobamos que el primer argumento sea "mycalc"
        if (strcmp("mycalc", argv_execvp[0]) == 0) {
            // Comprobamos que los demas argumentos no sean NULL
            // para poder realizar la operación
            if (argv_execvp[1] != NULL && argv_execvp[2] != NULL && argv_execvp[3] != NULL) {
                // Comprobamos si la operación deseada es la suma
                if (strcmp("add", argv_execvp[2]) == 0) {
                    // Convertimos los arrays de chars que nos llegan a ints
                    int x = atoi(argv_execvp[1]);
                    int y = atoi(argv_execvp[3]);

                    // Realizamos la suma y la guardamos en total
                    total = total + x + y;

                    // En este buffer guardaremos el resultado
                    char buffer[25];
                    sprintf(buffer, "%d", total);

                    // Puntero a buffer
                    const char * resultado = buffer;

                    // Cambiamos el entorno del buffer a "Acc"
                    if (setenv("Acc", resultado, 1) < 0) {
                        // Si es menor que 0 imprimimos error de cambio de entorno
                        perror("Error: cambio de entorno no válido\n");
                        goto error;
                    }

                    // Preparamos la salida con un array de caracteres de longitud 
                    char salida[65];
                    snprintf(salida, 65, "[OK] %d + %d = %d; Acc %s\n", x, y, x + y, getenv("Acc"));

                    if ((write(2, salida, strlen(salida))) < 0) {
                        perror("Error: No se puede escribir");
                        goto error;
                    }

                    // Comprobamos si la operación desada es el módulo
                } else if (strcmp("mod", argv_execvp[2]) == 0) {
                    int x = atoi(argv_execvp[1]);
                    int y = atoi(argv_execvp[3]);
                    char salida[65];

                    snprintf(salida, 65, "[OK] %d %% %d = %d * %d + %d\n", x, y, y, abs(floor(x / y)), x % y);
                    if ((write(2, salida, strlen(salida))) < 0) {
                        perror("Error: No se puede escribir");
                        goto error;
                    }
                    // Si no ha entradado en ninguna de las operaciones anteriores
                    // devolvemos un error.
                } else {
                    if ((write(1, "[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>",
                            strlen("[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>"))) < 0) {
                        perror("Error write");
                        goto error;
                    }
                }
            } else {
                if ((write(1, "[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>",
                        strlen("[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>"))) < 0) {
                    perror("Error write");
                    goto error;
                }
            }

            // Pasamos a definir mycp
            // Comprobamos que el primer argumento sea mycp
        } else if (strcmp(argv_execvp[0], "mycp") == 0) {
            if (argv_execvp[1] != NULL && argv_execvp[2] != NULL) {
                int descorigen = open(argv_execvp[1], O_RDONLY, 0644);
                if (descorigen >= 0) {
                    char buf[1024];
                    int descdestino = open(argv_execvp[2], O_TRUNC | O_WRONLY | O_CREAT, 0644);

                    // Vamos leyendo el fichero y lo sacamos por terminal con STDOUT_FILENO
                    int nread, nwrite;
                    while ((nread = read(descorigen, buf, 1024)) > 0) {

                        do {

                            nwrite = write(descdestino, buf, nread);
                            if (nwrite < 0) {
                                if (close(descdestino) < 0) {
                                    perror("Error al cerrar el fichero\n");
                                    goto error;
                                }
                                perror("Error al escribir en la linea de comandos\n");
                                goto error;
                            }
                            nread -= nwrite;

                        } while (nread > 0);
                    }
                    if (nread < 0) {
                        perror("Error al leer fichero\n");
                        if (close(descorigen) < 0) {
                            perror("Error al cerrar el fichero\n");
                        }
                    }
                    // Cuando acabamos de leer el fichero lo cerramos, tambien lo cerramos ante un error cuando lo tenemos abierto

                    if (close(descorigen) < 0) {
                        perror("Error al cerrar el fichero\n");
                    }
                    if (close(descdestino) < 0) {
                        perror("Error al cerrar el fichero\n");
                    }
                    char str[100];

                    snprintf(str, 100, "[OK] Copiado con exito el fichero %s a %s\n", argv_execvp[1], argv_execvp[2]);

                    write(1, str, strlen(str));
                    //Controlamos la correcta apertura del fichero
                } else {
                    if ((write(1, "[ERROR] Error al abrir el fichero origen : No such file or directory",
                            strlen("[ERROR] Error al abrir el fichero origen : No such file or directory"))) < 0) {
                        perror("Error en write");
                        goto error;
                    }
                }
                //Controlamos la sintaxis de mycp
            } else {
                if ((write(1, "[ERROR] La estructura del comando es mycp <fichero origen> <fichero destino>",
                        strlen("[ERROR] La estructura del comando es mycp <fichero origen> <fichero destino>"))) < 0) {
                    perror("Error en write");
                    goto error;
                }
            }
            //Comandos simples y redirecciones
        } else if (command_counter == 1) {
            int pid = fork();
            if (pid == -1) {
                perror("Error en fork: Reescribir");
                return (-1);
            }

            int filehandle = 0;
            int stat;
            //Hijo
            if (pid == 0) {
                //Redirecciones de salida, entrada y error
                if (strcmp(filev[1], "0") != 0) {

                    if ((close(1)) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }

                    if ((filehandle = open(filev[1], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                        perror("Error al abrir fichero\n");
                        goto error;
                    }
                }
                if (strcmp(filev[0], "0") != 0) {
                    if ((close(0)) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }
                    if ((filehandle = open(filev[0], O_RDWR, 0644)) < 0) {
                        perror("Error al abrir fichero\n");
                        goto error;
                    }
                }
                if (strcmp(filev[2], "0") != 0) {
                    if ((close(2)) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }
                    if ((filehandle = open(filev[2], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                        perror("Error al abrir fichero\n");
                        goto error;
                    }
                }
                //Hacemos que el hijo ejecute

                if (execvp(argv_execvp[0], argv_execvp) < 0) {
                    perror("Error al ejecutar\n");
                    goto error;
                }
            } else { //padre
                if (filehandle != 0) {
                    if ((close(filehandle)) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }
                }
                if (!in_background) {
                    while (wait( & stat) > 0);
                    if (stat < 0) {
                        perror("Error ejecucion hijo\n"); // Cambiar todos los errores por perror
                    }
                }
            }

        } else {
            int n = command_counter;
            int fd[2];
            int pid, status2;
            int filehandle = 0;

            int in ;

            if (( in = dup(0)) < 0) {
                perror("Error al duplicar descriptor\n");
                goto error;
            }

            for (int i = 0; i < n; i++) {
                //Creacion del siguiente pipe, si es el ultimo proceso, no se crea
                if (i != n - 1) {
                    if (pipe(fd) < 0) {
                        perror("Error en pipe\n");
                        exit(0);
                    }
                }

                /* se crea el proceso siguiente en la cadena */
                switch (pid = fork()) {

                case -1:
                    perror("Error en fork\n");

                    if ((close(fd[0])) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }
                    if ((close(fd[1])) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }
                    exit(0);
                    //Proceso hijo
                case 0:
                    //La entrada estandar pasa a ser la del hijo anterior, que viene dado por in

                    // En procesos intermedios, la redireccion de error es valida
                    if (strcmp(filev[2], "0") != 0) {
                        if ((close(2)) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }

                        if ((filehandle = open(filev[2], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                            perror("Error al abrir fichero\n");
                            goto error;
                        }
                    }

                    if (i == 0 && strcmp(filev[0], "0") != 0) {
                        if ((close(0)) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }
                        if ((filehandle = open(filev[0], O_RDWR, 0644)) < 0) {
                            perror("Error al abrir fichero\n");
                            goto error;
                        }
                    } else {
                        if ((close(0)) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }
                        if (dup( in ) < 0) {
                            perror("Error al duplicar descriptor\n");
                            goto error;
                        }
                        if ((close( in )) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }
                    }

                    // Si no es el ultimo proceso, cierra la salida estandar
                    if (i != n - 1) {

                        if ((close(1)) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }

                        if (dup(fd[1]) < 0) {
                            perror("Error al duplicar descriptor\n");
                            goto error;
                        }
                        if ((close(fd[0])) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }
                        if ((close(fd[1])) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }
                    } else {
                        if (strcmp(filev[1], "0") != 0) {
                            if ((close(1)) < 0) {
                                perror("Error al cerrar descriptor");
                                goto error;
                            }

                            if ((filehandle = open(filev[1], O_TRUNC | O_WRONLY | O_CREAT, 0644)) < 0) {
                                perror("Error al abrir fichero\n");
                                goto error;
                            }
                        }
                    }

                    getCompleteCommand(argvv, i);
                    if (in_background) {
                        printf("[%d]\n", getpid());
                    }

                    if (execvp(argv_execvp[0], argv_execvp) < 0) {
                        perror("Error al ejecutar\n");
                        goto error;
                    }
                    break;

                    //proceso padre
                default:
                    // El padre le da el nuevo valor a in para que pueda utilizarlo el hijo a no ser que sea el ultimo proceso

                    if ((close( in )) < 0) {
                        perror("Error al cerrar descriptor");
                        goto error;
                    }
                    if (i != n - 1) {
                        if (( in = dup(fd[0])) < 0) {
                            perror("Error al duplicar descriptor\n");
                            goto error;
                        }
                        if (dup(fd[0]) < 0) {
                            perror("Error al duplicar descriptor\n");
                            goto error;
                        }
                        if ((close(fd[1])) < 0) {
                            perror("Error al cerrar descriptor");
                            goto error;
                        }

                    }
                }
            }
            if (filehandle != 0) {

                if ((close(filehandle)) < 0) {
                    perror("Error al cerrar descriptor");
                    goto error;
                }
            }
            // Al terminar el bucle, el primer proceso espera al último, que irá despertando a todos
            if (!in_background) {
                while (wait( & status2) > 0);
                if (stat < 0) {
                    perror("Error ejecucion hijo\n"); // Cambiar todos los errores por perror
                }
            }
        }
    }
    return 0;
}