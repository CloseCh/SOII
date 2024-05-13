#include "directorios.h"

int lectura(char *ruta, char *dispositivo, char tipo, char flag);

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, RED"Sintaxis: mi_ls [flag] <dispositivo> <ruta>\n"RESET);
        exit(FALLO);
    }
    
    char *ruta;
    char *dispositivo;
    //Ver si tiene el flag correcto
    if (argc == 4 && strcmp(argv[1], "-l") == 0) {
        ruta = argv[3];
        dispositivo = argv[2];
        lectura(ruta, dispositivo, ruta[strlen(ruta)-1]!='/', 1);
        return EXITO;
    } else if (argc == 4 && strcmp(argv[1], "-l") != 0) {
        fprintf(stderr, RED"mi_ls: invalid option -- '%s'\n"RESET, argv[1]);
        exit(FALLO);
    }

    //Formato simple
    ruta = argv[2];
    dispositivo = argv[1];
    lectura(ruta, dispositivo, ruta[strlen(ruta)-1]!='/', 0);

    return EXITO;
}

int lectura(char *ruta, char *dispositivo, char tipo, char flag){
    bmount(dispositivo);

    char buffer[TAMBUFFER] = "";
    mi_dir(ruta, buffer, tipo, flag);
    printf("%s \n",buffer);

    bumount();

    return EXITO;
}