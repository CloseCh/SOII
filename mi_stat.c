#include "directorios.h"


int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, RED"Sintaxis: mi_stat <dispositivo> <ruta>\n"RESET);
        exit(FALLO);
    }

    char* ruta = argv[2];

    //montar el dispositivo virtual
    bmount(argv[1]);

    struct STAT estado;

    mi_stat(ruta, &estado);

    //Imprimir parametros del STAT
        printf ("Nº de inodo: %d\n", estado.nInodo);
        printf ("tipo: %c\n", estado.tipo);
        printf ("permisos: %d\n", estado.permisos);
        printf ("atime: %s", ctime(&estado.atime));
        printf ("ctime: %s", ctime(&estado.ctime));
        printf ("mtime: %s", ctime(&estado.mtime));
        printf ("nlinks: %d\n", estado.nlinks);
        printf ("tamEnBytesLog: %d\n", estado.tamEnBytesLog);
        printf ("numBloquesOcupados: %d\n", estado.numBloquesOcupados);

    bumount();

    return EXITO;
}