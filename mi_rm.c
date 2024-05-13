#include "directorios.h"

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        fprintf(stderr, RED "Sintaxis: ./mi_rm disco /ruta\n" RESET);
        exit(FALLO);
    }

    // Montamos el disco
    bmount(argv[1]);
    // No se ha de poder borrar el directorio raíz
    if(strcmp(argv[2],"/")==0){
        fprintf(stderr, RED "No se puede borrar el directorio raíz\n" RESET);
        exit(FALLO);
    } 
    // Llamada al  mi_unlink
    mi_unlink(argv[2]);
    // Desmontar el disco
    bumount(argv[1]);

    return EXITO;
}