#include "directorios.h"


int main(int argc, char **argv){
    if (argc < 4){
        fprintf(stderr, RED"Sintaxis: mi_mkdir <dispositivo> <permisos> </ruta>\n"RESET);
        exit(FALLO);
    }    

    int permisos = atoi(argv[2]);

    if (permisos < 0 || permisos > 7){
        fprintf(stderr, RED"permisos no valido, 0 <= permisos <= 7 \n"RESET);
        exit(FALLO);
    }

    //Comprobar que sea un directorio
    char* ruta = argv[3];

    if (ruta[strlen(ruta)-1] != '/'){
        fprintf(stderr, RED"No es un directorio\n"RESET);
        exit(FALLO);
    }

    //montar el dispositivo virtual
    bmount(argv[1]);

    mi_creat(ruta, permisos);

    bumount();

    return EXITO;
}