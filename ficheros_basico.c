#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
    int tamMB = (nbloques / 8) / BLOCKSIZE;
    if ((nbloques / 8) % BLOCKSIZE != 0){ //mirar si se requiere un bloque extra
        return ++tamMB;
    }
    return tamMB;
}

int tamAI(unsigned int ninodos){
    int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if ((ninodos * INODOSIZE) % BLOCKSIZE != 0){ //mirar si se requiere un bloque extra
       return ++tamAI;
    }
    return tamAI;
}

int initSB(unsigned int nbloques, unsigned int ninodos){
    struct superbloque SB;

    //Ponemos el valor inicial a cada atributo del objeto SB
    //Posisicion del primer bloque del mapa de bits
    SB.posPrimerBloqueMB       = posSB + tamSB;
    SB.posUltimoBloqueMB       = SB.posPrimerBloqueMB + tamMB(nbloques)-1;
    SB.posPrimerBloqueAI       = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI       = SB.posPrimerBloqueAI + tamAI(ninodos)-1;
    SB.posPrimerBloqueDatos    = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos    = nbloques-1;
    SB.posInodoRaiz            = 0;
    SB.posPrimerInodoLibre     = 0;
    SB.cantBloquesLibres       = nbloques;
    SB.cantInodosLibres        = ninodos;
    SB.totBloques              = nbloques;
    SB.totInodos               = ninodos;
    
    if(bwrite(posSB, &SB) == FALLO) return FALLO;
    
    return EXITO;
}

int initMB(){
    int sizeMB = BLOCKSIZE/8;
    char bufferMB[sizeMB];

    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) return FALLO;

    //Creamos una variable que contenga el tamaño de los metadatos
    int tamt = tamSB + SB.totBloques + SB.totInodos;
    int sobra = tamt % 8;

    //cantidad de bytes a 1
    tamt /= 8;

    for (int i = 0; i < tamt; i++){
        //Ponemos 1111111 en cada bloque
        bufferMB[i] = 255;
    }

    
    //Hay casos en los que puede que no sobre encontes lo contemplamos.
    if (sobra != 0){
        char cont = 0;
        //Los 1s restantes se colocaran en la siguiente posicion
        //Si tamt%8=3 --> en tamt/8+1 2^7+2^6+2^5
        while(sobra>0){
            cont+=2^(8-sobra);
            sobra--;
        }
        
        bufferMB[tamt + 1] = cont;
        //Rellenamos el resto con 0s
        for (int i = tamt + 2; i < sizeMB; i++){
            bufferMB[i] = 0;
        }
    } else {
        for (int i = tamt + 1; i < sizeMB; i++){
            bufferMB[i] = 0;
        }
    }

    //Cambiar el dato del SB.
    SB.cantBloquesLibres -= tamt-1;
    
    //Escribiendo en el disco
    if(bwrite(SB.posPrimerBloqueMB, bufferMB) == FALLO) return FALLO;

    return EXITO;
}

int initAI(){
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) return FALLO;
    

    //si hemos inicializado SB.posPrimerInodoLibre = 0 entonces
    int contInodos = SB.posPrimerInodoLibre + 1;

    //para cada bloque del AI
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){

        if (bread(i,inodos) == -1) return FALLO;

        //para cada inodo del bloque
        for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++){ 
            
            inodos[j].tipo = 'l';//libre

            //si no hemos llegado al último inodo del AI, entonces
            if (contInodos < SB.totInodos){

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
        if (bwrite(i,inodos) == -1) return FALLO;
    }

    return EXITO;
}
