#include "directorios.h"

int main(int argc, char **argv)
{

    if (argc < 4)
    {
        fprintf(stderr, RED "Sintaxis: ./mi_link disco /ruta_fichero_original /ruta_enlace\n" RESET);
        exit(FALLO);
    }

    // Montamos el disco
    bmount(argv[1]);
    // Llamada al  mi_link
    mi_link(argv[2], argv[3]);
    // Desmontar el disco
    bumount(argv[1]);

    return EXITO;
}