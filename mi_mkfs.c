#include <stdio.h>
#include <string.h>
#include "ficheros_basico.h"


int main(int argc, char **argv){
    //montar el dispositivo virtual
    bmount(argv[1]);

    //inicializar a 0s el fichero usado como dispositivo virtual
    int nbloques = atoi(argv[2]);

    unsigned char buf[BLOCKSIZE];

    memset(buf, '\0', BLOCKSIZE); // '\0' o '0'

    for(int i = 0; i < nbloques; i++){
        bwrite(i, buf); //escribir el bloque en el buffer
    }

    initSB(nbloques, nbloques/4);
    initMB();
    initAI();
    

    //desmontar el dispositivo virtual
    bumount();
}