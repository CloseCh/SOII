#include <stdio.h>
#include "bloques.h"


int main(int argc, char **argv){
    //montar el dispositivo virtual
    bmount(argv[1]);
    //inicializar a 0s el fichero usado como dispositivo virtual
    int nbloques=atoi(argv[2]);
    unsigned char buf[BLOCKSIZE];
    memset(buf,'\0',sizeof(char)); // '\0' o '0'
    for(int i=0;i<nbloques;i++){
        bwrite(i,buf); //escribir el bloque en el buffer
    }
    //desmontar el dispositivo virtual
    bumount();
}