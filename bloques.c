#include "bloques.h"

static int descriptor = 0;


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

int bumount(){
    return close(descriptor);
}

int bwrite(unsigned int nbloque, const void *buf){
    //calculamos desplazamiento
    int desplazamiento = nbloque * BLOCKSIZE;

    //movemos el puntero del fichero en el offset correcto
    lseek(descriptor, desplazamiento, SEEK_SET);

    //volcamos el contenido del buffer en la posicion del dv
    int size = write(descriptor, buf, BLOCKSIZE);
    if (size != BLOCKSIZE){
        perror(RED "Error"); printf(RESET);
        return FALLO;
    }
    return size;
}

int bread(unsigned int nbloque, void *buf){
    //calculamos desplazamiento
    int desplazamiento = nbloque * BLOCKSIZE;

    // movemos el puntero del fichero en el offset correcto
    lseek(descriptor, desplazamiento, SEEK_SET);

    int size = read(descriptor, buf, BLOCKSIZE);
    if (size != BLOCKSIZE){
        perror(RED "Error"); printf(RESET);
        return FALLO;
    }
    return size;
}