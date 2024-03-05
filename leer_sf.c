#include "ficheros_basico.h"



int main(int argc, char **argv){
    printf ("DATOS DEL SUPERBLOQUE \n");
    struct superbloque bufferSB;
    bread(posSB, &bufferSB);
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
}