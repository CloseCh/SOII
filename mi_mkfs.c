#include "directorios.h"


int main(int argc, char **argv){
    //montar el dispositivo virtual
    bmount(argv[1]);

    //Datos:
    unsigned int nbloques = atoi(argv[2]);
    unsigned int ninodos = nbloques/4;
    unsigned char buf[BLOCKSIZE];

    
    //inicializar a 0s el fichero usado como dispositivo virtual
    memset(buf, '\0', BLOCKSIZE); // '\0' o '0'
    for(int i = 0; i < nbloques; i++){
        bwrite(i, buf); //escribir el bloque en el buffer
    }

    initSB(nbloques, ninodos);
    initMB();
    initAI();
    
    //Prueba en reservar inodo
    reservar_inodo ('d', 7);

    //desmontar el dispositivo virtual
    bumount();
}