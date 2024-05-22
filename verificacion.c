#include "verificacion.h"


int main (int argc, char **argv){
    //Verificación de sintaxis
    if (argc < 3) {
        fprintf(stderr, RED "Sintaxis: ./verificacion <dispositivo> <directorio_simulación>\n" RESET);
        exit(FALLO);
    }

    //Declaración de variables
    struct STAT estado;
    char *dispositivo = argv[1];
    char *directorio = argv[2];
    int num_entradas; 

    //Verificar que es un directorio
    if (directorio[strlen(directorio)-1] != '/'){
        fprintf(stderr, RED"No es un directorio\n"RESET);
        exit(FALLO);
    }

    //Montar dispositivo
    bmount(dispositivo);

    //Leemos el STAT del directorio
    if (mi_stat(directorio, &estado) == FALLO)
        return FALLO;
    
    num_entradas = estado.tamEnBytesLog/sizeof(struct entrada);

    //Verificar cantidad de entradas del directorio
    if (num_entradas != NUMPROCESOS){
        fprintf(stderr, RED"Fallo, no coincide la cantidad de entradas con numero de procesos\n"RESET);
        return FALLO;
    }

    //Crear el fichero "informe.txt" dentro del directorio de simulación

    //Leer entradas del directorio de simulación

    //Bucle para cada directorio de un proceso

    

    //Desmontar dispositivo
    bumount();

    return EXITO;
}