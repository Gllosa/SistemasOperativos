
#include "queue.h"
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_CONSUMERS 1

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
//Creacion de mutex, variables condición y descriptor de fichero:
//buffer_controller controla el buffer
//fd controla el descriptor de fichero
pthread_mutex_t fd;
pthread_mutex_t buffer_controller;
pthread_cond_t empty;
pthread_cond_t full;
struct queue *cola;
const char *fichero;
FILE *pDescriptor;
int total = 0;

struct param {
  int id_ini;
  int op;
};

// Cada hilo tiene acceso a su propio dominio de id`s, donde el nº de operaciones que tendrá asociado será
// floor(num.lineas/num.productores), leerá e insertará de uno en uno
void *producir(void *arg) {
  // Sacar parametros
  struct param *p = arg;

  // Hallamos el descriptor que corresponde al primer indice, para que no se mezclen bloqueamos y desbloqueamos
  if(pthread_mutex_lock(&fd) < 0){
    perror("Error de mutex");
    exit(-1);
  }

  pDescriptor = fopen(fichero, "r");
  
  int counter = 0;
  char chr;
  while (counter < p->id_ini) {
    chr = fgetc(pDescriptor);
    if (chr == '\n') {
      counter++;
    }
  }
  // Almacenamos la posicion por la que va, de esta manera la podemos recuperar.
  FILE *current = pDescriptor;

  if(pthread_mutex_unlock(&fd) < 0){
    perror("Error de mutex");
    exit(-1);
  }

  int i, i1, i2 = 0;
  for (int j = p->op; j > 0; j--) {


    pDescriptor = current;
    if(fscanf(pDescriptor, "%d %d %d", &i, &i1, &i2) < 0){
      perror("Error al extraer datos archivo");
      exit(-1);
    }
    current = pDescriptor;


    struct element temporal = {i1, i2};

    if(pthread_mutex_lock(&buffer_controller) < 0){
      perror("Error de mutex");
      exit(-1);
    }
    while (queue_full(cola))
      if(pthread_cond_wait(&full, &buffer_controller) < 0){
        perror("Error de variable de condicion");
        exit(-1);
      }


    if(queue_put(cola, &temporal) < 0){
      perror("Error al insertar");
      exit(-1);
    }
    if(pthread_cond_signal(&empty) < 0){
      perror("Error de variable de condicion");
      exit(-1);
    }
    if(pthread_mutex_unlock(&buffer_controller) < 0){
      perror("Error de mutex");
      exit(-1);
    }
  }
  pthread_exit(0);
}

void *consumir(int *numValores) {
  struct element data;
  // Bucle de todo mientras no se hallan leido todas las ops esperadas.
  for (int k = 0; k < *numValores; k++) {

    if(pthread_mutex_lock(&buffer_controller) < 0){
      perror("Error de mutex");
      exit(-1);
    }

    while (queue_empty(cola))
      if(pthread_cond_wait(&empty, &buffer_controller) < 0){
        perror("Error de variable de condicion");
        exit(-1);
      }

    struct element *data = queue_get(cola);
    if(data==NULL){
      perror("Error al extraer");
      exit(-1);
    }

    switch (data->type) {
    case 1:
      total += 1 * data->time;

      break;
    case 2:
      total += 3 * data->time;

      break;
    case 3:
      total += 10 * data->time;

      break;
    default:
      perror("Valor no valido");
    }
    if(pthread_cond_signal(&full) < 0){
      perror("Error de variable de condicion");
      exit(-1);
    }
    if(pthread_mutex_unlock(&buffer_controller) < 0){
      perror("Error de mutex");
      exit(-1);
    }
  }
  pthread_exit(0);
}

int calculo_lineas(const char filename[]) {
  FILE *file = fopen(filename, "r");
  if(file == NULL){
    perror("Error al abrir fichero");
    exit(-1);
  }
  char chr;
  int lines = 0;
  while (!feof(file)) {
    chr = fgetc(file);
    if (chr == '\n') {
      lines++;
    }
  }
  fclose(file);
  return lines;
}

int main(int argc, const char *argv[]) {
  //Control de errores de los datos iniciales
  if (argc > 4) {
    perror("Numero de argumentos invalido");
    return -1;
  }

  FILE *descriptor = fopen(argv[1], "r");
  if(descriptor == NULL){
    perror("Error al abrir fichero");
    exit(-1);
  }

  int numVal;
  if(fscanf(descriptor, "%d", &numVal) < 0){
    perror("Error al extraer datos archivo");
    exit(-1);
  }
  int numLin = calculo_lineas(argv[1]);
  if (numVal > (numLin - 1)) {
    perror("Error: Se indica un numero de operaciones incorrecto");
    return -1;
  }

  int productores = atoi(argv[2]);
  if (productores <= 0) {
    perror("Error: Numero invalido de productores.");
    return -1;
  }
  int size = atoi(argv[3]);
  if (size <= 0) {
    perror("Error: Tamaño invalido.");
    return -1;
  }

  cola = queue_init(size);
  if(pthread_mutex_init(&buffer_controller, NULL) < 0){
    perror("Error inicializar variable de condicion");
    exit(-1);
  }
  if(pthread_mutex_init(&fd, NULL) < 0){
    perror("Error inicializar variable de condicion");
    exit(-1);
  }
  if(pthread_cond_init(&full, NULL) < 0){
    perror("Error inicializar mutex");
    exit(-1);
  }
  if(pthread_cond_init(&empty, NULL) < 0){
    perror("Error inicializar mutex");
    exit(-1);
  }
  //Creamos los hilos y establecemos el nº de operaciones que hará cada uno
  int operaciones = floor((numVal / productores));
  int id_inicio = 1;
  pthread_t hilosP[productores];
  pthread_t hiloC;
  if(pthread_create(&hiloC, NULL, (void *)consumir, &numVal) < 0){
    perror("Error al crear hilo");
    exit(-1);
  }
  //Ejecución de las operaciones
  int i;
  fichero = malloc(sizeof(char[strlen(argv[1])]));
  fichero = argv[1];
  struct param args[productores];
  for (i = 0; i < (productores - 1); i++) {
    args[i].op = operaciones;
    args[i].id_ini = id_inicio;

    if(pthread_create(&hilosP[i], NULL, (void *)producir, &args[i]) < 0){
      perror("Error al crear hilo");
      exit(-1);
    }

    id_inicio += operaciones;
  }
  int op_ultimo = numVal - (i * operaciones);
  args[productores - 1].op = op_ultimo;
  args[productores - 1].id_ini = id_inicio;

  //Control de errores de operaciones y valores finales
  if(pthread_create(&hilosP[productores - 1], NULL, (void *)producir, &args[productores - 1]) < 0){
    perror("Error al crear hilo");
    exit(-1);
  }

  for (int i = 0; i < productores; i++) {
    if(pthread_join(hilosP[i], NULL) < 0){
      perror("Error al esperar al hilo");
      exit(-1);
    }
  }
  if(pthread_join(hiloC, NULL) < 0){
    perror("Error al esperar al hilo");
    exit(-1);
  }

  printf("Total: %i €.\n", total);
  queue_destroy(cola);
  if(pthread_mutex_destroy(&fd) < 0){
    perror("Error al destruir mutex");
    exit(-1);
  }
  if(pthread_mutex_destroy(&buffer_controller) < 0){
    perror("Error al destruir mutex");
    exit(-1);
  }
  if(  pthread_cond_destroy(&full) < 0){
    perror("Error al destruir variable condicion");
    exit(-1);
  }
  if(pthread_cond_destroy(&empty) < 0){
    perror("Error al destruir variable condicion");
    exit(-1);
  }
  if(fclose(pDescriptor) < 0){
    perror("Error al cerrar descriptor");
    exit(-1);
  }
  if(fclose(descriptor) < 0){
    perror("Error al cerrar descriptor");
    exit(-1);
  }
  return 0;
}
