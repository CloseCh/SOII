#include "ficheros.h"

int main(int argc, char **argv){
    if(argc < 4){
        fprintf(stderr, RED"Sintaxis: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n"RESET);
        exit(FALLO);
    }
    bmount(argv[1]);

    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);

    if(nbytes==0){
        liberar_inodo(ninodo);
    }else{
        mi_truncar_f(ninodo, nbytes);
    }

    fprintf(stderr, "DATOS INODO %d:\n", ninodo);
    //Leer inodo e guardar en una variable
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    fprintf(stderr, "tipo=%c\n",inodo.tipo);
    fprintf(stderr, "permisos=%d\n",inodo.permisos);
    fprintf(stderr, "atime: %s",ctime(&inodo.atime));
    fprintf(stderr, "ctime: %s",ctime(&inodo.ctime));
    fprintf(stderr, "mtime: %s",ctime(&inodo.mtime));
    fprintf(stderr, "nlinks=%d\n",inodo.nlinks);
    fprintf(stderr, "tamEnBytesLog=%d\n",inodo.tamEnBytesLog);
    fprintf(stderr, "numBloquesOcupados=%d\n",inodo.numBloquesOcupados);
    
    bumount();
}