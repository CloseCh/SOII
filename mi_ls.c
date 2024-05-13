#include "directorios.h"

#define TAMFILA 100
#define TAMBUFFER (TAMFILA*1) //suponemos un máx de 1000 entradas, aunque debería ser SB.totInodos

int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, RED"Sintaxis: mi_ls [extension] <dispositivo> <ruta>\n"RESET);
        exit(FALLO);
    }

    char* ruta = argv[2];

    //montar el dispositivo virtual
    bmount(argv[1]);

    //Formato simple
    char buffer[TAMBUFFER] = "";

    mi_dir(ruta, buffer);

    printf("%s\n", buffer);
    fflush(stdout);
    bumount();

    return EXITO;
}