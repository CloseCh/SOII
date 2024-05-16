#include "directorios.h"

int main(int argc, char **argv) {

    if (argc < 3) {
        fprintf(stderr, RED "Sintaxis: ./mi_rmdir <dispositivo> </ruta_directorio>\n" RESET);
        exit(FALLO);
    }

    char *ruta = argv[2];
    char *dispositivo = argv[1];

    //Comprobar que es un directorio
    if (ruta[strlen(ruta)-1] != '/'){
        fprintf(stderr, RED"No es un fichero\n"RESET);
        exit(FALLO);
    }

    // No se ha de poder borrar el directorio raíz
    if(strcmp(ruta, "/")==0){
        fprintf(stderr, RED "No se puede borrar el directorio raíz\n" RESET);
        exit(FALLO);
    } 
    
    // Montamos el disco
    bmount(dispositivo);

    // Llamada al  mi_unlink
    if (mi_unlink(ruta) == FALLO)
        return FALLO;

    // Desmontar el disco
    bumount();

    return EXITO;
}