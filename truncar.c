#include "ficheros.h"

int main(int argc, char **argv){
    if(argc < 4){
        fprintf(stderr, RED"Sintaxis: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n"RESET);
        exit(FALLO);
    }
    bmount(argv[1]);
    if(argv[3]==0){
        liberar_inodo(argv[2]);
    }else{
        mi_truncar_f(argv[2],argv[3]);
    }
    bumount();
}