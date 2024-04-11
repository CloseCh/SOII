#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Inicial entre las 2 primeras barrasa, final el resto
    //Separar inicial de final y poner que tipo (si acaba en / es directorio, si no fichero)
    int cont=1;
    int ch='/';
    char *aux =camino+1; //Quitamos la primera barra
    //Mientras no lo encuentre o se salga
    while(camino[cont]!='/' && camino[cont]!='\0'){
        cont++;
        
    }
    //Si no hay segunda barra, inicial =camino
    if(camino[cont]=='\0'){
        inicial=aux; //Copiamos camino 
        tipo="f";
        final="";
        return EXITO;
    }

    if(camino[cont]=='/'){
        strncpy(inicial,aux,cont-1); //Copiamos hasta segunda /
        tipo="d";
        final=strchr(aux,ch); //Copiamos a partir de segunda /
        return EXITO;
    }

    return FALLO;

}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, 
                    unsigned int *p_entrada, char reservar, unsigned char permisos){

}



void mostrar_error_buscar_entrada(int error) {
    // fprintf(stderr, "Error: %d\n", error);
    switch (error) {
        case -2: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
        case -3: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
        case -4: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
        case -5: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
        case -6: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
        case -7: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
        case -8: fprintf(stderr, "Error: No es un directorio.\n"); break;
    }
}
