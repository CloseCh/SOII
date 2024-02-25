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
 *   ---
 *   
 *   unsigned int nbloques: ---
 *  
 *   unsigned int ninodos: ---
 *
 *   returns: ---
 */
int initSB(unsigned int nbloques, unsigned int ninodos){

}

/*
 * Function: initMB
 * ----------------------------
 *   ---
 *
 *   returns: ---
 */
int initMB(){

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