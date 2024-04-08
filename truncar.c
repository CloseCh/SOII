#include "ficheros.h"

int main(int argc, char **argv){
    if(argc < 4){
        fprintf(stderr, RED"Sintaxis: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n"RESET);
        exit(FALLO);
    }
    if (bmount(argv[1]) == FALLO) exit(FALLO);

    unsigned int ninodo = atoi(argv[2]);
    unsigned int nbytes = atoi(argv[3]);

    if(nbytes==0){
        if (liberar_inodo(ninodo) == FALLO) exit(FALLO);
    }else{
        if (mi_truncar_f(ninodo, nbytes) == FALLO) exit(FALLO);
    }

    fprintf(stderr, "DATOS INODO %d:\n", ninodo);
    //Leer inodo e guardar en una variable
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) exit(FALLO);
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