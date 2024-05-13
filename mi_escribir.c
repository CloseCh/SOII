#include "directorios.h"

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr, RED "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n" RESET);
        exit(FALLO);
    }

    int escritos=0;
    // Montamos el disco
    bmount(argv[1]);

    // miramos longitud de texto
    unsigned int length = strlen(argv[3]);
    fprintf(stdout, "longitud texto: %d\n", length);

    //FALTA COMPROBAR SI ES UN FICHERO 

    escritos=mi_write(argv[2], argv[3], argv[4], length);
    fprintf(stderr, "Bytes escritos: %d\n", escritos);
    // Desmontar el disco
    bumount(argv[1]);

    return EXITO;
}