#include "directorios.h"

#define OFFSET1 9000        //BL 8
#define OFFSET2 209000      //BL 204
#define OFFSET3 30725000    //BL 30.004
#define OFFSET4 409605000   //BL 400.004
#define OFFSET5 480000000   //BL 468.750

int tipo_simple_inodo(char texto[], unsigned int length);
int tipo_multiple_inodo(char texto[], unsigned int length);
int escribir(struct inodo *bufferInodo, char texto[], int pos, unsigned int length, int offset, int inodoR);
int leer_inodo_reservar();

int main(int argc, char **argv){
    if (argc < 4){
        fprintf(stderr, RED
            "Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n"RESET);
        fprintf(stderr, RED
            "Offsets: %d, %d, %d, %d, %d\n"RESET,
            OFFSET1,OFFSET2,OFFSET3,OFFSET4,OFFSET5);
        fprintf(stderr, RED
            "Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n"RESET);

        exit(FALLO);
    }

    //Montamos el disco
    bmount(argv[1]);

    //miramos longitud de texto
    unsigned int length = strlen(argv[2]);
    fprintf(stdout, "longitud texto: %d\n\n",length);

    //Miramos de que tipo es la declaración y llamar funciones
    int tipo = atoi(argv[3]);
    if (tipo == 0) {
        tipo_simple_inodo(argv[2], length);
    } else if (tipo == 1) {
        tipo_multiple_inodo(argv[2], length);
    } else {
        fprintf(stderr, RED
            "diferentes_inodos debe ser 0 o 1"RESET);
        exit(FALLO);
    }

    bumount(argv[1]);
}

//retorna los inodos reservados
int tipo_simple_inodo(char texto[], unsigned int length){
    struct inodo bufferInodo;
    int pos;

    //Vemos cual es el que queremos reservar
    int reservado = leer_inodo_reservar();

    //reservamos inodo
    pos = reservar_inodo('f',6);
    if(leer_inodo(pos, &bufferInodo) == FALLO) return FALLO;

    /********************OFFSET1************************/
    escribir(&bufferInodo,texto,pos,length,OFFSET1,reservado);
    /********************OFFSET2************************/
    escribir(&bufferInodo,texto,pos,length,OFFSET2,reservado);
    /********************OFFSET3************************/
    escribir(&bufferInodo,texto,pos,length,OFFSET3,reservado);
    /********************OFFSET4************************/
    escribir(&bufferInodo,texto,pos,length,OFFSET4,reservado);
    /********************OFFSET5************************/
    escribir(&bufferInodo,texto,pos,length,OFFSET5,reservado);

    return EXITO;
}

//retorna los inodos reservados
int tipo_multiple_inodo(char texto[], unsigned int length){
    struct inodo bufferInodo;
    int pos;
    int reservado;

    /********************OFFSET1************************/
    reservado = leer_inodo_reservar();
    pos = reservar_inodo('f',6);
    if(leer_inodo(pos, &bufferInodo) == FALLO) return FALLO;
    escribir(&bufferInodo,texto,pos,length,OFFSET1,reservado);

    /********************OFFSET2************************/
    reservado = leer_inodo_reservar();
    pos = reservar_inodo('f',6);
    if(leer_inodo(pos, &bufferInodo) == FALLO) return FALLO;
    escribir(&bufferInodo,texto,pos,length,OFFSET2,reservado);

    /********************OFFSET3************************/
    reservado = leer_inodo_reservar();
    pos = reservar_inodo('f',6);
    if(leer_inodo(pos, &bufferInodo) == FALLO) return FALLO;
    escribir(&bufferInodo,texto,pos,length,OFFSET3,reservado);

    /********************OFFSET4************************/
    reservado = leer_inodo_reservar();
    pos = reservar_inodo('f',6);
    if(leer_inodo(pos, &bufferInodo) == FALLO) return FALLO;
    escribir(&bufferInodo,texto,pos,length,OFFSET4,reservado);

    /********************OFFSET5************************/
    reservado = leer_inodo_reservar();
    pos = reservar_inodo('f',6);
    if(leer_inodo(pos, &bufferInodo) == FALLO) return FALLO;
    escribir(&bufferInodo,texto,pos,length,OFFSET5,reservado);

    return EXITO;
}

int escribir(struct inodo *bufferInodo, char texto[], int pos, 
            unsigned int length, int offset, int inodo_reservado){
    
    fprintf(stderr, "Nº inodo reservado: %d\n",inodo_reservado);
    fprintf(stderr, "offset: %d\n", offset);

    int escrito;
    struct STAT stat;
    //insertar texto en inodo
    escrito = mi_write_f(pos,texto,offset,length);
    fprintf(stderr, "Bytes escritos: %d\n", escrito);
    //Ver el estado del inodo
    mi_stat_f(pos, &stat);
    
    fprintf(stderr, "stat.tamEnBytesLog=%d\n", stat.tamEnBytesLog);
    fprintf(stderr, "stat.numBloquesOcupados=%d\n", stat.numBloquesOcupados);
    fprintf(stderr, "\n");

    return EXITO;
}

int leer_inodo_reservar(){
    struct superbloque SB;

    //Vemos cual es el que queremos reservar
    if (bread(posSB, &SB) == FALLO) return FALLO;
    return SB.posPrimerInodoLibre;
}