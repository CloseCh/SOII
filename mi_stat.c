#include "directorios.h"


int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, RED"Sintaxis: mi_stat <dispositivo> <ruta>\n"RESET);
        exit(FALLO);
    }

    char* ruta = argv[2];

    //montar el dispositivo virtual
    bmount(argv[1]);

    struct stat estado;

    mi_stat(ruta, &estado);

    bumount();

    return EXITO;
}