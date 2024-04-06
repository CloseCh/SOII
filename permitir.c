#include <stdio.h>
#include <string.h>
#include "ficheros.h"


int main(int argc, char **argv){
    if (argc < 4){
        fprintf(stderr, RED"Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>"RESET);
        exit(FALLO);
    }

    bmount(argv[1]);

    int ninodo = atoi(argv[2]);
    int permiso = atoi(argv[3]);
    mi_chmod_f(ninodo, permiso);

    bumount();

    return EXITO;
}