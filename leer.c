#include <stdio.h>
#include <string.h>
#include "ficheros.h"

int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, RED"Sintaxis: ./leer <nombre_dispositivo> <ninodo>\n"RESET);
        exit(FALLO);
    }
    bmount(argv[1]);

    unsigned int offset, leidos, ninodo, total_leido;
    leidos = offset = total_leido = 0;
    ninodo = atoi(argv[2]);

    //Leemos el inodo para saber cuantos bytes hay que leer

    int tambuffer = BLOCKSIZE;
    char buffer_texto[tambuffer];
    
    memset(buffer_texto, 0, tambuffer);
    leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
    while (leidos > 0){
        write(1,buffer_texto,leidos);
        offset += tambuffer;
        total_leido += leidos;
        memset(buffer_texto, 0, tambuffer);
        leidos = mi_read_f(ninodo, buffer_texto, offset, tambuffer);
    }

    char string[128];
    sprintf(string, "total_leidos %d\n", total_leido);
    write(2, string, strlen(string));

    bumount(argv[1]);

    return EXITO;
}