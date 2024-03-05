#include "ficheros_basico.h"

/*****************************************************************************************/
/*                                       NIVEL 2                                         */
/*****************************************************************************************/

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
    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) return FALLO;

    //Miramos la ocupacion de los metadatos
    unsigned int metaData = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);

    //Con esto miramos si se debe escribir en un solo bloque o más
    unsigned int usedBlock = metaData / 8 / BLOCKSIZE;

    //contenedor de un bloque del mapa de bytes
    char bufferMB[BLOCKSIZE];

    //Bucle para escribir los bloques ocupados por metadatos
    for(int i = 0; i < usedBlock; i++){
        //Se escribe en el disco
        if(bwrite(SB.posPrimerBloqueMB + i, bufferMB) == FALLO) return FALLO;
    }

    //Verificar las sobras que no ocupan 1 byte
    int sobra = metaData % 8;

    //cantidad de bytes a 1
    metaData /= 8;

    for (int i = 0; i < metaData; i++){
        //Ponemos 1111111 en cada bloque
        bufferMB[i] = 255;
    }
        
    //Los 1s restantes se colocaran en la siguiente posicion
    char cont = 0;
    unsigned char mascara = 128;    // 10000000
    while(sobra>0){
        bufferMB[metaData] |= mascara;
        mascara >>= 1;
        sobra--;
    }

    //Cambiar el dato del SB.
    SB.cantBloquesLibres -= metaData-1;

    //Escribiendo en el disco
    if(bwrite(SB.posPrimerBloqueMB + usedBlock, bufferMB) == FALLO) return FALLO;

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


/*****************************************************************************************/
/*                                       NIVEL 3                                         */
/*****************************************************************************************/

int escribir_bit(unsigned int nbloque, unsigned int bit){

}

char leer_bit(unsigned int nbloque){

}

int reservar_bloque(){

}

int liberar_bloque(unsigned int nbloque){

}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo){

}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){

}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
    
}