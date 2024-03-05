#include <stdio.h>
#include <string.h>
#include "ficheros_basico.h"

int main(int argc, char **argv){
    //montar el dispositivo virtual
    bmount(argv[1]);

    //leer el superbloque
    struct superbloque bufferSB;
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }

    //Imprimir datos de SB
    printf ("DATOS DEL SUPERBLOQUE \n");
    printf ("posPrimerBloqueMB = %i \n", bufferSB.posPrimerBloqueMB);
    printf ("posUltimoBloqueMB = %i \n", bufferSB.posUltimoBloqueMB);
    printf ("posPrimerBloqueAI = %i \n", bufferSB.posPrimerBloqueAI);
    printf ("posUltimoBloqueAI = %i \n", bufferSB.posUltimoBloqueAI);
    printf ("posPrimerBloqueDatos = %i \n", bufferSB.posPrimerBloqueDatos);
    printf ("posUltimoBloqueDatos = %i \n", bufferSB.posUltimoBloqueDatos);
    printf ("posInodoRaiz = %i \n", bufferSB.posInodoRaiz);
    printf ("posPrimerInodoLibre = %i \n", bufferSB.posPrimerInodoLibre);
    printf ("cantBloquesLibres = %i \n", bufferSB.cantBloquesLibres);
    printf ("cantInodosLibres = %i \n", bufferSB.cantInodosLibres);
    printf ("totBloques = %i \n", bufferSB.totBloques);
    printf ("totInodos = %i \n", bufferSB.totInodos);

    //Ver los bytes ocupados por cada estructura
    printf ("\n");
    printf ("sizeof struct superbloque: %ld\n", sizeof(bufferSB));

    //Leemos el primer bloque de inodos
    struct inodo bufferAI[BLOCKSIZE/INODOSIZE];
    if(bread(bufferSB.posPrimerBloqueAI, &bufferAI) == FALLO){
        fprintf(stderr, RED
            "Error: lectura AI\n"RESET);
        return FALLO;
    }
    printf ("sizeof struct inodo: %ld\n", sizeof(bufferAI[0]));
    printf ("\n");
    
    //Leer lista enlazada de los inodos:
    //printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES");
    //unsigned int i = 1;
    //unsigned int cantidadAI = bufferSB.posUltimoBloqueAI - bufferSB.posPrimerBloqueAI + 1;
    //while(i <= cantidadAI){
    //    for(int j = 0; j < BLOCKSIZE/INODOSIZE; j++){
    //        printf("%d ",bufferAI[j].punterosDirectos[0]);
    //    }
    //    //Leer el siguiente bloque AI
    //    if(bread(bufferSB.posPrimerBloqueAI + i, &bufferAI) == FALLO){
    //        fprintf(stderr, RED
    //            "Error: lectura AI\n"RESET);
    //        return FALLO;
    //    }
    //    i++;
    //}
    printf ("\n");

    //desmontar el dispositivo virtual
    bumount();
}