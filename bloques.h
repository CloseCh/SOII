// bloques.h

#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()


#define BLOCKSIZE 1024 // bytes


#define EXITO 0 //para gestión errores
#define FALLO -1 //para gestión errores


#define BLACK   "\x1B[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1B[37m"
#define ORANGE  "\x1B[38;2;255;128;0m"
#define ROSE    "\x1B[38;2;255;151;203m"
#define LBLUE   "\x1B[38;2;53;149;240m"
#define LGREEN  "\x1B[38;2;17;245;120m"
#define GRAY    "\x1B[38;2;176;174;174m"
#define RESET   "\x1b[0m"


#define NEGRITA "\x1b[1m"

//Debuging de cada nivel


#define DEBUGN3     0
#define DEBUGN4     0
#define DEBUGN6     0
#define DEBUGN7     1
#define DEBUGN8     1
#define DEBUGN9     1
#define DEBUGN10    1

/*
 * Function: bmount
 * ----------------------------
 *   Función para montar el dispositivo virtual, dado que se consta de un 
 *   fichero, esta funcion consiste en abrir lo.
 *
 *   const char *camino: puntero a un array de tipo char con la localización 
 *   del fichero.
 *
 *   returns: descriptor del fichero si todo va bien, -1 si error.
 */
int bmount(const char *camino);

/*
 * Function: bumount
 * ----------------------------
 *   Llamar a la función close() para liberar el espacio ocupado por el 
 *   descriptor de fichero.
 *
 *   returns: 0 si ha ido todo bien, -1 si error.
 */
int bumount();

/*
 * Function: bwrite
 * ----------------------------
 *   Escribe 1 bloque en el dispositivo virtual, en el bloque físico 
 *   especificado por nbloque.
 *
 *   unsigned int nbloque: indica la posicion del dispositivo virtual a 
 *   introducir el contenido.
 *   const void *buf: puntero hacia el contenido de un buffer de memoria a
 *   volcar. 
 *
 *   returns: tamaño del bloque si todo va bien, -1 si error.
 */
int bwrite(unsigned int nbloque, const void *buf);

/*
 * Function: bread
 * ----------------------------
 *   Lee 1 bloque del dispositivo virtual, que se corresponde con el bloque
 *   físico especificado por nbloque
 *   
 *   unsigned int nbloque: especifica el bloque a realizar la lectura.
 * 
 *   void *buf: puntero hacia los nbytes (BLOCKSIZE) contenidos a partir de 
 *   la posición (nº de byte) del dispositivo virtual correspondiente al nº de 
 *   bloque. Este contenido es el que se vuelca en un buffer de memoria.
 *
 *   returns: tamaño del bloque si todo va bien, -1 si error.
 */
int bread(unsigned int nbloque, void *buf);