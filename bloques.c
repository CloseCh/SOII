#include "bloques.h"

static int descriptor = 0;

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
int bmount(const char *camino){
    //No aparecen permisos desenmascarar
    umask(000);
    //open(camino,oflags,mode)
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    if (descriptor == -1) {          
        perror(RED "Error"); printf(RESET);
        return FALLO;
    }

    return descriptor;
}

/*
 * Function: bumount
 * ----------------------------
 *   Llamar a la función close() para liberar el espacio ocupado por el 
 *   descriptor de fichero.
 *
 *   returns: 0 si ha ido todo bien, -1 si error.
 */
int bumount(){
    return close(descriptor);
}

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
int bwrite(unsigned int nbloque, const void *buf){
    //calculamos desplazamiento
    int desplazamiento = nbloque * BLOCKSIZE;

    //movemos el puntero del fichero en el offset correcto
    lseek(descriptor, desplazamiento, nbloque);

    //volcamos el contenido del buffer en la posicion del dv
    if (write(descriptor, buf, BLOCKSIZE) != BLOCKSIZE){
        perror(RED "Error"); printf(RESET);
        return FALLO;
    }
    return BLOCKSIZE;
}

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
int bread(unsigned int nbloque, void *buf){
    //calculamos desplazamiento
    int desplazamiento = nbloque * BLOCKSIZE;

    // movemos el puntero del fichero en el offset correcto
    lseek(descriptor, desplazamiento, nbloque);

    if (read(descriptor, buf, BLOCKSIZE) != BLOCKSIZE){
        perror(RED "Error"); printf(RESET);
        return FALLO;
    }
    return BLOCKSIZE;
}