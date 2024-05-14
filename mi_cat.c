#include "directorios.h"


int main(int argc, char **argv){
     if (argc < 3){
        fprintf(stderr, RED"Sintaxis: mi_cat <disco> </ruta_fichero>\n"RESET);
        exit(FALLO);
    }

    
    bmount(argv[1]);

    unsigned int offset, leidos, ninodo, total_leido;
    leidos = offset = total_leido = 0;
    ninodo = atoi(argv[2]);

    //Leemos el inodo
    struct inodo inodo;
    if(leer_inodo(ninodo,&inodo) == FALLO) return FALLO;

    unsigned int tambuffer = 1500;
    char buffer_texto[tambuffer];
    
    memset(buffer_texto, 0, tambuffer);
    leidos = mi_read(argv[2], buffer_texto, offset, tambuffer);
    if (leidos == -1) {
        char string[256];
        sprintf(string, "\ntotal_leidos 0\ntamEnBytesLog %d\n",inodo.tamEnBytesLog);
        write(2, string, strlen(string));
        exit(FALLO);
    }



    while (leidos > 0){
        write(1,buffer_texto,leidos);
        offset += tambuffer;
        total_leido += leidos;
        memset(buffer_texto, 0, tambuffer);
        leidos = mi_read(argv[2], buffer_texto, offset, tambuffer);
    }

    char string[256];
    //Caso que no coinciden los bytes leidos con el tamaño en bytes logico
    //Falta poner fprint
    //Hay que filtrar la basura
    if(total_leido!=inodo.tamEnBytesLog){
        //fprint();
        exit(FALLO);
    }
    sprintf(string, "\ntotal_leidos %d\ntamEnBytesLog %d \n", total_leido,inodo.tamEnBytesLog);
    write(2, string, strlen(string));

    bumount();

    return EXITO;



}