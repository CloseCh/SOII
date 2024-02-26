#include "ficheros_basico.h"

struct superbloque SB [1];
struct inodo inodos [BLOCKSIZE/INODOSIZE];

/*
 * Function: tamMB
 * ----------------------------
 *   ---
 *   
 *   unsigned int nbloques: ---
 *
 *   returns: ---
 */
int tamMB(unsigned int nbloques){

}

/*
 * Function: tamAI
 * ----------------------------
 *   ---
 *   
 *   unsigned int ninodos: ---
 *
 *   returns: ---
 */
int tamAI(unsigned int ninodos){

}

/*
 * Function: initSB
 * ----------------------------
 *   Inicializamos el superbloque con los valores iniciales y pasados 
 *   por parametro.
 *   
 *   unsigned int nbloques: ---
 *  
 *   unsigned int ninodos: ---
 *
 *   returns: ---
 */
int initSB(unsigned int nbloques, unsigned int ninodos){
    //Ponemos el valor inicial a cada atributo del objeto SB
    SB->posPrimerBloqueMB=posSB+tamSB;
    SB->posUltimoBloqueMB=SB->posUltimoBloqueMB+tamMB(nbloques)-1;
    SB->posPrimerBloqueAI=SB->posUltimoBloqueMB+1;
    SB->posUltimoBloqueAI=SB->posPrimerBloqueAI+tamAI(ninodos)-1;
    SB->posPrimerBloqueDatos=SB->posUltimoBloqueAI+1;
    SB->posUltimoBloqueDatos=nbloques-1;
    SB->posInodoRaiz=0;
    SB->posPrimerInodoLibre=0;
    SB->cantBloquesLibres=nbloques;
    SB->cantInodosLibres=ninodos;
    SB->totBloques=nbloques;
    SB->totInodos=ninodos;
}

/*
 * Function: initMB
 * ----------------------------
 *   ---
 *
 *   returns: ---
 */
int initMB(){
    char bufferMB[BLOCKSIZE/8];
    //Creamos una variable que contenga el tamaño de los metadatos
    int tamt=tamSB+tamMB+tamAI;
    for (int i = 0; i < (tamt)/8; i++){
        //Ponemos 1111111 en cada bloque
        bufferMB[i]=255;
    }
    int sobra=tamt%8;
    char cont=0;
    //Los 1s restantes se colocaran en la siguiente posicion
    //Si tamt%8=3 --> en tamt/8+1 2^7+2^6+2^5
    while(sobra>0){
        cont+=2^(8-sobra);
        sobra--;
    }
    bufferMB[tamt/8+1]= cont;
    //Rellenamos el resto con 0s
    for (int i = tamt/8+2; i < BLOCKSIZE/8; i++){
        bufferMB[i]=0;
    }
    //Falta salvar el bufferMB en la posición correspondiente
    


    

}

/*
 * Function: initAI
 * ----------------------------
 *   se encargará de inicializar la lista de inodos libres
 *
 *   returns: ---
 */
int initAI(){
    //si hemos inicializado SB.posPrimerInodoLibre = 0 entonces
    int contInodos = SB->posPrimerInodoLibre + 1;

    //para cada bloque del AI
    for (int i = SB->posPrimerBloqueAI; i <= SB->posUltimoBloqueAI; i++){

        unsigned char buf[BLOCKSIZE];
        if (bread(i,buf) == -1) {
            perror(RED "Error"); printf(RESET);
            return FALLO;
        }

        //para cada inodo del bloque
        for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++){ 
            
            inodos[j].tipo = 'l';//libre

            //si no hemos llegado al último inodo del AI, entonces
            if (contInodos < SB->totInodos){

                //enlazamos con el siguiente
                inodos[j].punterosDirectos[0] = contInodos; 
                contInodos++;

            //hemos llegado al último inodo
            } else { 
                inodos[j].punterosDirectos[0] = UINT_MAX;
                /*hay que salir del bucle, el último bloque no tiene por qué 
                estar completo !!!*/
                break;
            }
        }
        if (bwrite(i,buf) == -1) {
            perror(RED "Error"); printf(RESET);
            return FALLO;
        }
    }
}
