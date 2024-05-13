#include "directorios.h"

#define TAMFILA 100
#define TAMBUFFER (TAMFILA*1000) //suponemos un máx de 1000 entradas, aunque debería ser SB.totInodos

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, RED"Sintaxis: mi_ls [flag] <dispositivo> <ruta>\n"RESET);
        exit(FALLO);
    }
    
    char *ruta;
    char *dispositivo;
    //Ver si tiene el flag correcto
    if (argc == 4 && strcmp(argv[2], "-l") != 0) {
        ruta = argv[3];
        dispositivo = argv[2];
        lectura(ruta, dispositivo, ruta[strlen(ruta)-1]!='/', 1);
        return EXITO;
    } else {
        fprintf(stderr, RED"mi_ls: invalid option -- '%d'\n"RESET, argv[2]);
        exit(FALLO);
    }

    //Caso basico
    ruta = argv[2];
    dispositivo = argv[1];
    lectura(ruta, dispositivo, ruta[strlen(ruta)-1]!='/', 0);

    return EXITO;
}

int lectura(char *ruta, char *dispositivo, char tipo, char flag){
    bmount(dispositivo);

    char buffer[TAMBUFFER] = "";
    mi_dir(ruta, buffer, tipo, flag);

    bumount();

    return EXITO;
}