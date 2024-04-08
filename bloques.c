#include "bloques.h"

static unsigned int descriptor = 0;


int bmount(const char *camino){
    //No aparecen permisos desenmascarar
    umask(000);
    //open(camino,oflags,mode)
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    if (descriptor == -1) {
        fprintf(stderr, RED"Error: creacion de disco\n"RESET);
        return FALLO;
    }

    return descriptor;
}

int bumount(){
    return close(descriptor);
}

int bwrite(unsigned int nbloque, const void *buf){
    //calculamos desplazamiento
    unsigned int desplazamiento = nbloque * BLOCKSIZE;

    //movemos el puntero del fichero en el offset correcto
    lseek(descriptor, desplazamiento, SEEK_SET);

    //volcamos el contenido del buffer en la posicion del dv
    unsigned int size = write(descriptor, buf, BLOCKSIZE);

    /*En cada funcion posterior se escribirá por terminal 
    de donde proviene*/
    if (size != BLOCKSIZE) {
        fprintf(stderr, RED"Error: escribir bloque\n"RESET);
        return FALLO;
    }
    
    return size;
}

int bread(unsigned int nbloque, void *buf){
    //calculamos desplazamiento
    unsigned int desplazamiento = nbloque * BLOCKSIZE;

    // movemos el puntero del fichero en el offset correcto
    lseek(descriptor, desplazamiento, SEEK_SET);

    unsigned int size = read(descriptor, buf, BLOCKSIZE);

    /*En cada funcion posterior se escribirá por terminal 
    de donde proviene*/
    if (size != BLOCKSIZE) {
        fprintf(stderr, RED"Error: leer bloque\n"RESET);
        return FALLO;
    }
    
    return size;
}