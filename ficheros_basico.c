#include "ficheros_basico.h"

/*****************************************************************************************/
/*                                       NIVEL 2                                         */
/*****************************************************************************************/

int tamMB(unsigned int nbloques){
    unsigned int tamMB = (nbloques / 8) / BLOCKSIZE;
    if ((nbloques / 8) % BLOCKSIZE != 0){ //mirar si se requiere un bloque extra
        return ++tamMB;
    }
    return tamMB;
}

int tamAI(unsigned int ninodos){
    unsigned int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
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
    
    if(bwrite(posSB, &SB) == FALLO) {
        fprintf(stderr, RED"Error: escritura superbloque en initSB\n"RESET);
        return FALLO;
    }
    
    return EXITO;
}

int initMB(){
    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB en inicializacion de MB\n"RESET);
        return FALLO;
    }

    //Miramos la ocupacion de los metadatos
    unsigned int metaData = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);

    //Cambiar el dato del SB.
    SB.cantBloquesLibres -= metaData;

    //Con esto miramos si se debe escribir en un solo bloque o más
    unsigned int usedBlock = metaData / 8 / BLOCKSIZE;

    //contenedor de un bloque del mapa de bytes
    unsigned char bufferMB[BLOCKSIZE];

    //Bucle para escribir los bloques ocupados por metadatos
    for(int i = 0; i < usedBlock; i++){
        memset(bufferMB, 255, BLOCKSIZE);
        //Se escribe en el disco
        if(bwrite(SB.posPrimerBloqueMB + i, bufferMB) == FALLO){
            fprintf(stderr, RED
                "Error: escritura en inicializacion del MB 1\n"RESET);
            return FALLO;
        }
    }

    //limpiar el buffer para post relleno.
    memset(bufferMB, '\0', BLOCKSIZE);

    //Verificar las sobras que no ocupan 1 byte
    unsigned int sobra = metaData % 8;

    //cantidad de bytes a 1
    metaData /= 8;

    metaData -= usedBlock*1024;

    for (int i = 0; i < metaData; i++){
        //Ponemos 1111111 en cada bloque
        bufferMB[i] = 255;
    }
        
    //Los 1s restantes se colocaran en la siguiente posicion
    unsigned char mascara = 128;    // 10000000
    while(sobra>0){
        bufferMB[metaData] |= mascara;
        mascara >>= 1;
        sobra--;
    }

    //Escribiendo en el disco
    if(bwrite(SB.posPrimerBloqueMB + usedBlock, bufferMB) == FALLO){
        fprintf(stderr, RED
            "Error: escritura MB en inicializacion del MB 2\n"RESET);
        return FALLO;
    }

    //Escritura del superbloque modificado
    if(bwrite(posSB, &SB) == FALLO){
        fprintf(stderr, RED
            "Error: escritura SB en inicializacion del MB 2\n"RESET);
        return FALLO;
    }

    return EXITO;
}

int initAI(){
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB en inicializacion de AI\n"RESET);
        return FALLO;
    }
    

    //si hemos inicializado SB.posPrimerInodoLibre = 0 entonces
    unsigned int contInodos = SB.posPrimerInodoLibre + 1;

    //para cada bloque del AI
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){

        if (bread(i,inodos) == FALLO) {
            fprintf(stderr, RED
                "Error: lectura AI en inicializacion de AI\n"RESET);
            return FALLO;
        }

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

        if (bwrite(i,inodos) == FALLO){
            fprintf(stderr, RED
                "Error: escritura AI en inicializacion de AI\n"RESET);
            return FALLO;
        }
    }

    return EXITO;
}


/*****************************************************************************************/
/*                                       NIVEL 3                                         */
/*****************************************************************************************/
int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en escribir bit\n"RESET);
        return FALLO;
    }

    //Calculamos posiciones de memoria en MB
    unsigned int posbyte = nbloque/8;
    unsigned int posbit = nbloque%8;

    //Ver en que bloque del MB se debe escribir
    unsigned int nbloqueMB = posbyte/BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB+nbloqueMB;

    //Leemos el nbloque y lo cargamos en bufferMB
    unsigned char bufferMB[BLOCKSIZE];
    if (bread(nbloqueabs, bufferMB) == -1) {
        fprintf(stderr, RED
            "Error: lectura MB en escribir bit\n"RESET);
        return FALLO;
    }

    posbyte = posbyte%BLOCKSIZE; //Ver en que byte del bloque es
    unsigned char mascara = 128; //10000000
    mascara >>= posbit; //Desplazamos posbits el bit de la máscara
    if (bit == 1) {
        bufferMB[posbyte]|=mascara;
    } else if (bit == 0) {
        bufferMB[posbyte]&= ~mascara;
    } else {
        return FALLO;
    }

    //Escribimos el buffer en el dispositivo virtual
    if (bwrite(nbloqueabs, bufferMB) == FALLO) {
        fprintf(stderr, RED
            "Error: escritura MB en escribir bit\n"RESET);
        return FALLO;
    }

    return EXITO;
}

char leer_bit(unsigned int nbloque){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en leer bit\n"RESET);
        return FALLO;
    }

    //Calculamos posiciones de memoria en MB
    unsigned int posbyte = nbloque/8;
    unsigned int posbit = nbloque%8;

    //Ver en que bloque del MB se debe escribir
    unsigned int nbloqueMB = posbyte/BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB+nbloqueMB;

    //Leemos el nbloque y lo cargamos en bufferMB
    unsigned char bufferMB[BLOCKSIZE];
    if (bread(nbloqueabs,bufferMB) == -1) {
        fprintf(stderr, RED
            "Error: lectura MB en leer bit\n"RESET);
        return FALLO;
    }
    unsigned int posbyteA = posbyte%BLOCKSIZE; //Ver en que byte del bloque es

    unsigned char mascara = 128; // 10000000
    mascara >>= posbit; // desplazamiento de bits a la derecha, los que indique posbit
    mascara &= bufferMB[posbyteA]; // operador AND para bits
    mascara >>= (7-posbit); // desplazamiento de bits a la derecha

    fprintf(stderr, GRAY
        "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): %d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n" RESET,
        nbloque, posbyte, posbyteA, posbit, nbloqueMB, nbloqueabs);

    return mascara;
}

int reservar_bloque(){
    //Lectura SB
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en reservar bloque\n"RESET);
        return FALLO;
    }

    // Ver si quedan bloques libres
    if(SB.cantBloquesLibres < 1){
        fprintf(stderr, RED
            "Error: No quedan bloques libres\n"RESET);
        return FALLO;
    }

    //Localizar el primer bloque libre
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE]; //Usado en la comparación de bloques
    memset(bufferAux, 255, BLOCKSIZE);
    unsigned int nbloqueMB = 0;
    for (;;){
        if (bread(SB.posPrimerBloqueMB+nbloqueMB, bufferMB) == FALLO){
            fprintf(stderr, RED
                "Error: lectura MB en reservar bloque\n"RESET);
            return FALLO;
        }

        //Salir del bucle si se localiza
        if (memcmp(bufferMB, bufferAux, BLOCKSIZE) < 0)break;

        //Control de errores, salir si supera el maximo de bloques del MB
        if (SB.posPrimerBloqueMB+nbloqueMB > SB.posUltimoBloqueMB) {
            fprintf(stderr, RED
                "Error: superado maximo de bloques MB en reservar bloque\n"RESET);
            return FALLO;
        }

        nbloqueMB++;
    }

    //Iteracion para encontrar el byte del bloque menor a 255
    unsigned int posbyte = 0;
    while(posbyte < BLOCKSIZE){
        if(bufferMB[posbyte]<255) break;
        posbyte++;
    }
    //Control de errores, FALLO si se sale del rango de un bloque
    if (posbyte > BLOCKSIZE) {
        fprintf(stderr, RED
            "Error: superado maximo de bytes sobre un bloque en reservar bloque\n"RESET);
        return FALLO;
    }
        
    //Iteracion para encontrar bit a 0
    unsigned char mascara=128;
    unsigned int posbit = 0;
    while(bufferMB[posbyte] & mascara){
        bufferMB[posbyte]<<=1;
        posbit++;
    }

    //Determinar el numero de bloque físico
    unsigned int nbloque = (nbloqueMB*BLOCKSIZE+posbyte)*8+posbit;

    escribir_bit(nbloque,1);

    //Decrementamos la cantidad de bloques libres
    SB.cantBloquesLibres--;

    //Guardar el cambio de SB
    if (bwrite(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: escritura SB en reservar bloque\n"RESET);
        return FALLO;
    }

    //Grabamos un buffer de 0s en la posicion nbloques del disp
    memset(bufferAux,0,BLOCKSIZE);
    if (bwrite(nbloque, bufferAux) == FALLO) {
        fprintf(stderr, RED
            "Error: escritura SB en reservar bloque\n"RESET);
        return FALLO;
    }

    return nbloque;
}

int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en liberar bloque\n"RESET);
        return FALLO;
    }

    if (escribir_bit(nbloque,0) == FALLO) {
        return FALLO;
    }

    //Incrementamos cantidad de bloques libres
    SB.cantBloquesLibres++; //Falta salvar el superbloque
    //Guardar el cambio de SB
    if (bwrite(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: escritura SB en reservar bloque\n"RESET);
        return FALLO;
    }

    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo){
    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en escribir inodo\n"RESET);
        return FALLO;
    }

    //Obtener posición absoluta del dispositivo
    unsigned int nbloqueAI = (ninodo*INODOSIZE)/BLOCKSIZE;
    unsigned int nbloqueabs = nbloqueAI+SB.posPrimerBloqueAI;

    //Lectura bloque en memoria
    struct inodo inodos [BLOCKSIZE/INODOSIZE];
    if(bread(nbloqueabs, inodos) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura inodo en escribir inodo\n"RESET);
        return FALLO;
    }

    //Escribir contenido del inodo en el lugar correspondiente del array
    unsigned int posinodo = ninodo%(BLOCKSIZE/INODOSIZE);
    inodos[posinodo] = *inodo;

    // escribir bloque modificado en el dispositivo virtual
    if(bwrite(nbloqueabs,inodos) == FALLO) {
        fprintf(stderr, RED
            "Error: escritura sobre AI en escribir inodo\n"RESET);
        return FALLO;
    }

    return EXITO;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en leer inodo\n"RESET);
        return FALLO;
    }

    //Obtener posición absoluta del dispositivo
    int nbloqueAI = (ninodo*INODOSIZE)/BLOCKSIZE;
    int nbloqueabs = nbloqueAI+SB.posPrimerBloqueAI;

    //Lectura bloque en memoria
    struct inodo inodos [BLOCKSIZE/INODOSIZE];
    if(bread(nbloqueabs, inodos) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura inodo en leer inodo\n"RESET);
        return FALLO;
    }

    //Volcar el inodo de referencia
    unsigned int posinodo = ninodo%(BLOCKSIZE/INODOSIZE);
    *inodo = inodos[posinodo];

    return EXITO;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
    //Lectura del superbloque
    struct superbloque SB;
    if(bread(posSB, &SB) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura SB en reservar inodo\n"RESET);
        return FALLO;
    }

    //Comprobar si hay inodos libres
    if(SB.cantInodosLibres < 1) {
        fprintf(stderr, RED
            "Error: No quedan inodos libres\n"RESET);
        return FALLO;
    }

    //Actualizar lista enlazada de inodos libres
    unsigned int posInodoReservado = SB.posPrimerInodoLibre;
    struct inodo inodoAux;
    if(leer_inodo(posInodoReservado, &inodoAux) == FALLO) {
        fprintf(stderr, RED
            "Error: lectura de inodo en reservar inodo\n"RESET);
        return FALLO;
    }
    SB.posPrimerInodoLibre = inodoAux.punterosDirectos[0];

    //Inicializar todos los campos del inodo al que apuntaba inicialmente el superbloque
    struct inodo nuevoInodo;
    nuevoInodo.tipo = tipo;
    nuevoInodo.permisos = permisos;
    nuevoInodo.nlinks = 1;
    nuevoInodo.tamEnBytesLog = 0;
    nuevoInodo.atime = nuevoInodo.mtime = nuevoInodo.ctime = time(NULL);
    nuevoInodo.numBloquesOcupados = 0;

    for (int i = 0; i < 12; i++) {
        nuevoInodo.punterosDirectos[i] = 0;
    }

    for (int i = 0; i < 3; i++) {
        nuevoInodo.punterosIndirectos[i] = 0;
    }

    //Escribir el inodo inicializado en la posición del que era el primer inodo libre
    if (escribir_inodo(posInodoReservado, &nuevoInodo) == FALLO) {
        fprintf(stderr, RED
            "Error: escritura de inodo en reservar inodo\n"RESET);
        return FALLO;
    }

    // Decrementar la cantidad de inodos libres y rescribir el superbloque
    SB.cantInodosLibres--;
    if(bwrite(posSB,&SB)==FALLO){
        fprintf(stderr, RED
            "Error: escritura de SB en reservar inodo\n"RESET);
        return FALLO;
    }

    return posInodoReservado;
}

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr){
    //Asigno valor y luego retorno una vez y no todo el rato (lo cambio si no te gusta)
    //Apunto ptr a la posicion que toque
    if(nblogico<DIRECTOS){
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }else if(nblogico<INDIRECTOS0){
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }else if(nblogico<INDIRECTOS1){
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }else if(nblogico<INDIRECTOS2){
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }else{
        *ptr = 0;
        fprintf(stderr, RED
            "Error: Bloque lógico fuera de rango\n"RESET);
        return FALLO;
    }
    
    
}


int obtener_indice(unsigned int nblogico, int nivel_punteros){
    if (nblogico < DIRECTOS){
        return nblogico;
    }else if(nblogico < INDIRECTOS0){
        return nblogico-DIRECTOS;
    }else if(nblogico < INDIRECTOS1){
        if(nivel_punteros==2){
            return (nblogico-INDIRECTOS0)/NPUNTEROS;
        }else if(nivel_punteros==1){
            return (nblogico-INDIRECTOS0)%NPUNTEROS;
        }
    }else if(nblogico < INDIRECTOS2){
        if(nivel_punteros==3){
            return (nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);
        }else if(nivel_punteros==2){
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;
        }else if(nivel_punteros==1){
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
        }
    }
    return FALLO;
}


int traducir_bloque_inodo(struct inodo *inodo, unsigned int nblogico, unsigned char reservar){
    //Declarar variables
    unsigned int ptr, ptr_ant;
    int nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];

    ptr = ptr_ant = 0;
    nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr);//0:D, 1:I0, 2:I1, 3:I2
    nivel_punteros = nRangoBL;//el nivel_punteros +alto es el que cuelga directamente del inodo

    //iterar para cada nivel de punteros indirectos
    while(nivel_punteros > 0){
        //no cuelgan bloques de punteros
        if(ptr == 0){
            // bloque inexistente -> no imprimir error por pantalla!!!
            if(reservar == 0){ 
                return FALLO;
            //reservar bloques de punteros y crear enlaces desde el inodo hasta el bloque de datos
            }else{ 
                ptr = reservar_bloque();//de punteros
                inodo->numBloquesOcupados++;
                inodo->ctime = time(NULL);//fecha actual
                //el bloque cuelga directamente del inodo
                if(nivel_punteros == nRangoBL){
                    inodo->punterosIndirectos[nRangoBL-1] = ptr;
                    #if DEBUGN4
                        fprintf(stderr, GRAY
                            "[traducir_bloque_inodo()→ inodo.punterosIndirectos[%d] = %d (reservado BF %d para punteros_nivel%d)]\n"RESET,
                            nRangoBL-1, ptr, ptr, nivel_punteros);
                    #endif
                //el bloque cuelga de otro bloque de punteros
                }else{ 
                    buffer[indice] = ptr;
                    #if DEBUGN4
                        fprintf(stderr, GRAY
                            "[traducir_bloque_inodo()→ punteros_nivel%d [%d] = %d (reservado BF %d para punteros_nivel%d)]\n"RESET,
                            nivel_punteros, indice, ptr, ptr, nivel_punteros);
                    #endif
                    //salvamos en el dispositivo el buffer de punteros modificado
                    if(bwrite(ptr_ant, buffer) == FALLO){
                        fprintf(stderr, RED"Error: escritura1 de indice en traducir bloque inodo\n"RESET);
                        return FALLO;
                    }
                }
                memset(buffer, 0, BLOCKSIZE); //ponemos a 0 todos los punteros del buffer 
            }
        //leemos del dispositivo el bloque de punteros ya existente
        }else{
            if(bread(ptr, buffer) == FALLO){
                fprintf(stderr, RED"Error: lectura de indice en traducir bloque inodo\n"RESET);
                return FALLO;
            }
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr; //guardamos el puntero actual
        ptr = buffer[indice]; // y lo desplazamos al siguiente nivel 
        nivel_punteros--;
    } //al salir de este bucle ya estamos al nivel de datos

    //no existe bloque de datos
    if(ptr == 0){
        //error lectura ∄ bloque -> no imprimir error por pantalla!!!
        if(reservar == 0){
            return FALLO;
        }else{
            ptr = reservar_bloque();//de datos
            inodo->numBloquesOcupados++;
            inodo->ctime = time(NULL);
            //si era un puntero Directo
            if(nRangoBL == 0){
                //asignamos la direción del bl. de datos en el inodo
                inodo->punterosDirectos[nblogico] = ptr;
                #if DEBUGN4
                    fprintf(stderr, GRAY
                        "[traducir_bloque_inodo()→ inodo.punterosDirectos[%d] = %d (reservado BF %d para BL %d)]\n\n"RESET,
                        nblogico, ptr, ptr, nblogico);
                #endif
            }else{
                //asignamos la dirección del bloque de datos en el buffer
                buffer[indice] = ptr;
                #if DEBUGN4
                    fprintf(stderr, GRAY
                        "[traducir_bloque_inodo()→ punteros_nivel%d [%d] = %d (reservado BF %d para BL %d)]\n\n"RESET,
                        nivel_punteros, indice, ptr, ptr, nblogico);
                #endif
                 //salvamos en el dispositivo el buffer de punteros modificado
                if(bwrite(ptr_ant, buffer) == FALLO){
                    fprintf(stderr, RED"Error: escritura2 de indice en traducir bloque inodo\n"RESET);
                    return FALLO;
                }
            }
        }
    }

    return ptr;
}   