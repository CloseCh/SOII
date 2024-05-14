#include "directorios.h"

int main(int argc, char **argv)
{
    if (argc < 5) {
        fprintf(stderr, RED "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n" RESET);
        exit(FALLO);
    }

    char *ruta = argv[2];   //El segundo es la ruta del fichero
    //Tras obtener la ruta comprobar que es un fichero
    if (ruta[strlen(ruta)-1] == '/'){
        fprintf(stderr, RED "La ruta debe ser un fichero\n" RESET);
        exit(FALLO);
    }

    char *dispositivo = argv[1]; //El primero es el disco
    int offset = atoi(argv[argc-1]); //El ultimo indica el offset
    
    int escritos = 0;   //Para guardar la cantidad de bit escritos
    
    unsigned int length = 0; //Obtener la longitud del texto
    // miramos longitud de texto`
    for (int i = 3; i < argc-1; i++) {
        length += strlen(argv[i]); 
        length++; //el espacio en blanco ocupa un espacio
    }
    //Omitir el ultimo espacio en blanco
    length--;

    //Tras saber la longitud de texto se genera un arreglo del texto a escribir
    char texto[length];
    memset(texto, 0, length); // Limpiamos la basura que haya
    for (int i = 3; i < argc-1; i++) {
        strcat(texto, argv[i]);
        strcat(texto, " ");
    }
    
    fprintf(stdout, "longitud texto: %d\n", length);

    // Montamos el disco
    bmount(dispositivo);

    escritos = mi_write(ruta, texto, offset, length);

    fprintf(stderr, "Bytes escritos: %d\n", escritos);

    // Desmontar el disco
    bumount(argv[1]);

    return EXITO;
}