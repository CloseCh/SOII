#include "directorios.h"


int main(int argc, char **argv){
    if (argc < 4){
        fprintf(stderr, RED"Sintaxis: mi_mkdir <disco> <permisos> </ruta>\n"RESET);
        exit(FALLO);
    }    

    int permisos = atoi(argv[2]);

    if (permisos < 0 || permisos > 7){
        fprintf(stderr, RED"permisos no valido, , 0 <= permisos <= 7 \n"RESET);
        exit(FALLO);
    }

    //montar el dispositivo virtual
    bmount(argv[1]);

    mi_creat(argv[3], permisos);

    bumount();

    return EXITO;
}