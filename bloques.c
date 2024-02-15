#include "bloques.h"

static int descriptor = 0;

/*
 * Function: bmount
 * ----------------------------
 *   ---
 *
 *   const char *camino: .
 *
 *   returns: .
 */
int bmount(const char *camino){

    //open(camino,oflags,mode)
    return open(camino,O_RDWR,0666);
}

/*
 * Function: bumount
 * ----------------------------
 *   ---
 *
 *   returns: .
 */
int bumount(){
    
    return close(descriptor);
}

/*
 * Function: bwrite
 * ----------------------------
 *   ---
 *
 *   unsigned int nbloque: numero de bloque.
 *   const void *buf: Contenedor 
 *
 *   returns: .
 */
int bwrite(unsigned int nbloque, const void *buf){
    //lseek(descriptor, desp, origen)
    // movemos el puntero del fichero en el offset correcto
    off_t lseek(descriptor,nbloque*BLOCKSIZE,nbloque);
    //volcamos el contenido del buffer en la posicion del dv
    //write(descrptor, buffer, bytes)
    return size_t write(descriptor,buf,BLOCKSIZE);

}

/*
 * Function: bread
 * ----------------------------
 *   ---
 *
 *   unsigned int nbloque: .
 *   void *buf: 
 *
 *   returns: .
 */
int bread(unsigned int nbloque, void *buf){
    // movemos el puntero del fichero en el offset correcto
    off_t lseek(descriptor,nbloque*BLOCKSIZE,nbloque);
    //igual que el write
    return size_t read(descriptor,buf,BLOCKSIZE);


}