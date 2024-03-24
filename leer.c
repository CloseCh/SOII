#include <stdio.h>
#include <string.h>
#include "ficheros.h"

int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, RED"Sintaxis: ./leer <nombre_dispositivo> <ninodo>\n"RESET);
        exit(FALLO);
    }
    bmount(argv[1]);

    unsigned int offset, leidos, ninodo;
    unsigned int tambuffer = BLOCKSIZE;
    char buffer_texto[tambuffer];

    leidos = offset = 0;
    ninodo = atoi(argv[2]);

    memset(buffer_texto, 0, tambuffer);
    leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
    while (leidos > 0){
        write(1, buffer_texto, leidos);
        offset += tambuffer;
        memset(buffer_texto, 0, tambuffer);
        leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
    }

    char string[128];
    sprintf(string, "total_leidos %d\n", leidos);
    write(2, string, strlen(string));

    bumount(argv[1]);

    return EXITO;
}