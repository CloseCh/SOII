#include "ficheros_basico.h"

/*****************************************************************************************/
/*                                       NIVEL 2                                         */
/*****************************************************************************************/

int tamMB(unsigned int nbloques){
    unsigned int tamMB = (nbloques / 8) / BLOCKSIZE;
    if ((nbloques / 8) % BLOCKSIZE != 0) { //mirar si se requiere un bloque extra
        return ++tamMB;
    }
    return tamMB;
}

int tamAI(unsigned int ninodos){
    unsigned int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if ((ninodos * INODOSIZE) % BLOCKSIZE != 0) { //mirar si se requiere un bloque extra
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
    
    if (bwrite(posSB, &SB) == FALLO) return FALLO;
    
    return EXITO;
}

int initMB(){
    //Lectura del superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //Miramos la ocupacion de los metadatos
    unsigned int metaData = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);

    //Cambiar el dato del SB.
    SB.cantBloquesLibres -= metaData;

    //Con esto miramos si se debe escribir en un solo bloque o más
    unsigned int usedBlock = metaData / 8 / BLOCKSIZE;

    //contenedor de un bloque del mapa de bytes
    unsigned char bufferMB[BLOCKSIZE];

    //Bucle para escribir los bloques ocupados por metadatos
    for (int i = 0; i < usedBlock; i++) {
        memset(bufferMB, 255, BLOCKSIZE);
        //Se escribe en el disco
        if (bwrite(SB.posPrimerBloqueMB + i, bufferMB) == FALLO) return FALLO;
    }

    //limpiar el buffer para post relleno.
    memset(bufferMB, '\0', BLOCKSIZE);

    //Verificar las sobras que no ocupan 1 byte
    unsigned int sobra = metaData % 8;

    //cantidad de bytes a 1
    metaData /= 8;
    metaData -= usedBlock*1024;

    for (int i = 0; i < metaData; i++) {
        //Ponemos 1111111 en cada bloque
        bufferMB[i] = 255;
    }
        
    //Los 1s restantes se colocaran en la siguiente posicion
    unsigned char mascara = 128;    // 10000000
    while (sobra>0) {
        bufferMB[metaData] |= mascara;
        mascara >>= 1;
        sobra--;
    }

    //Escribiendo en el disco
    if (bwrite(SB.posPrimerBloqueMB + usedBlock, bufferMB) == FALLO) return FALLO;

    //Escritura del superbloque modificado
    if (bwrite(posSB, &SB) == FALLO) return FALLO;

    return EXITO;
}

int initAI(){
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    //Lectura del superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;
    

    //si hemos inicializado SB.posPrimerInodoLibre = 0 entonces
    unsigned int contInodos = SB.posPrimerInodoLibre + 1;

    //para cada bloque del AI
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {

        if (bread(i,inodos) == FALLO) return FALLO;

        //para cada inodo del bloque
        for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++) { 
            
            inodos[j].tipo = 'l';//libre

            //si no hemos llegado al último inodo del AI, entonces
            if (contInodos < SB.totInodos) {

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

        if (bwrite(i,inodos) == FALLO) return FALLO;
    }

    return EXITO;
}


/*****************************************************************************************/
/*                                       NIVEL 3                                         */
/*****************************************************************************************/

int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //Calculamos posiciones de memoria en MB
    unsigned int posbyte = nbloque / 8;
    unsigned int posbit = nbloque % 8;

    //Ver en que bloque del MB se debe escribir
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    //Leemos el nbloque y lo cargamos en bufferMB
    unsigned char bufferMB[BLOCKSIZE];
    if (bread(nbloqueabs, bufferMB) == -1) return FALLO;

    posbyte = posbyte % BLOCKSIZE; //Ver en que byte del bloque es
    unsigned char mascara = 128; //10000000
    mascara >>= posbit; //Desplazamos posbits el bit de la máscara
    if (bit == 1) {
        bufferMB[posbyte] |= mascara;
    } else if (bit == 0) {
        bufferMB[posbyte] &= ~mascara;
    } else {
        return FALLO;
    }

    //Escribimos el buffer en el dispositivo virtual
    if (bwrite(nbloqueabs, bufferMB) == FALLO) return FALLO;

    return EXITO;
}

char leer_bit(unsigned int nbloque){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //Calculamos posiciones de memoria en MB
    unsigned int posbyte = nbloque / 8;
    unsigned int posbit = nbloque % 8;

    //Ver en que bloque del MB se debe escribir
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    //Leemos el nbloque y lo cargamos en bufferMB
    unsigned char bufferMB[BLOCKSIZE];
    if (bread(nbloqueabs,bufferMB) == -1) return FALLO;
    unsigned int posbyteA = posbyte % BLOCKSIZE; //Ver en que byte del bloque es

    unsigned char mascara = 128; // 10000000
    mascara >>= posbit; // desplazamiento de bits a la derecha, los que indique posbit
    mascara &= bufferMB[posbyteA]; // operador AND para bits
    mascara >>= (7 - posbit); // desplazamiento de bits a la derecha

    #if DEBUGN3
        fprintf(stderr, GRAY
            "[leer_bit(%d)→ posbyte:%d, posbyte (ajustado): %d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d)]\n" RESET,
            nbloque, posbyte, posbyteA, posbit, nbloqueMB, nbloqueabs);
    #endif
    return mascara;
}

int reservar_bloque(){
    //Lectura SB
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

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
        if (bread(SB.posPrimerBloqueMB + nbloqueMB, bufferMB) == FALLO) return FALLO;

        //Salir del bucle si se localiza
        if (memcmp(bufferMB, bufferAux, BLOCKSIZE) < 0)break;

        //Control de errores, salir si supera el maximo de bloques del MB
        if (SB.posPrimerBloqueMB + nbloqueMB > SB.posUltimoBloqueMB) {
            fprintf(stderr, RED
                "Error: superado maximo de bloques MB en reservar bloque\n"RESET);
            return FALLO;
        }

        nbloqueMB++;
    }

    //Iteracion para encontrar el byte del bloque menor a 255
    unsigned int posbyte = 0;
    while (posbyte < BLOCKSIZE) {
        if (bufferMB[posbyte] < 255) break;
        posbyte++;
    }
    //Control de errores, FALLO si se sale del rango de un bloque
    if (posbyte > BLOCKSIZE) {
        fprintf(stderr, RED
            "Error: superado maximo de bytes sobre un bloque en reservar bloque\n"RESET);
        return FALLO;
    }
        
    //Iteracion para encontrar bit a 0
    unsigned char mascara = 128;
    unsigned int posbit = 0;
    while(bufferMB[posbyte] & mascara){
        bufferMB[posbyte] <<= 1;
        posbit++;
    }

    //Determinar el numero de bloque físico
    unsigned int nbloque = (nbloqueMB * BLOCKSIZE + posbyte) * 8 + posbit;

    if (escribir_bit(nbloque, 1) == FALLO) return FALLO;

    //Decrementamos la cantidad de bloques libres
    SB.cantBloquesLibres--;

    //Guardar el cambio de SB
    if (bwrite(posSB, &SB) == FALLO) return FALLO;

    //Grabamos un buffer de 0s en la posicion nbloques del disp
    memset(bufferAux,0,BLOCKSIZE);
    if (bwrite(nbloque, bufferAux) == FALLO) return FALLO;

    return nbloque;
}

int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    if (escribir_bit(nbloque, 0) == FALLO) return FALLO;

    //Incrementamos cantidad de bloques libres
    SB.cantBloquesLibres++; //Falta salvar el superbloque
    //Guardar el cambio de SB
    if (bwrite(posSB, &SB) == FALLO) return FALLO;

    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo){
    //Lectura del superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //Obtener posición absoluta del dispositivo
    unsigned int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
    unsigned int nbloqueabs = nbloqueAI + SB.posPrimerBloqueAI;

    //Lectura bloque en memoria
    struct inodo inodos [BLOCKSIZE / INODOSIZE];
    if (bread(nbloqueabs, inodos) == FALLO) return FALLO;

    //Escribir contenido del inodo en el lugar correspondiente del array
    unsigned int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
    inodos[posinodo] = *inodo;

    // escribir bloque modificado en el dispositivo virtual
    if (bwrite(nbloqueabs,inodos) == FALLO) return FALLO;

    return EXITO;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    //Lectura del superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //Obtener posición absoluta del dispositivo
    int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
    int nbloqueabs = nbloqueAI+SB.posPrimerBloqueAI;

    //Lectura bloque en memoria
    struct inodo inodos [BLOCKSIZE/INODOSIZE];
    if (bread(nbloqueabs, inodos) == FALLO) return FALLO;

    //Volcar el inodo de referencia
    unsigned int posinodo = ninodo % (BLOCKSIZE  /INODOSIZE);
    *inodo = inodos[posinodo];

    return EXITO;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
    //Lectura del superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //Comprobar si hay inodos libres
    if (SB.cantInodosLibres < 1) {
        fprintf(stderr, RED
            "Error: No quedan inodos libres\n"RESET);
        return FALLO;
    }

    //Actualizar lista enlazada de inodos libres
    unsigned int posInodoReservado = SB.posPrimerInodoLibre;
    struct inodo inodoAux;
    if (leer_inodo(posInodoReservado, &inodoAux) == FALLO) {
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

    for (int i = 0; i < 12; i++) 
        nuevoInodo.punterosDirectos[i] = 0;

    for (int i = 0; i < 3; i++) 
        nuevoInodo.punterosIndirectos[i] = 0;

    //Escribir el inodo inicializado en la posición del que era el primer inodo libre
    if (escribir_inodo(posInodoReservado, &nuevoInodo) == FALLO) return FALLO;

    // Decrementar la cantidad de inodos libres y rescribir el superbloque
    SB.cantInodosLibres--;
    if (bwrite(posSB, &SB)==FALLO) return FALLO;

    return posInodoReservado;
}

/*****************************************************************************************/
/*                                       NIVEL 4                                         */
/*****************************************************************************************/

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr){
    //Asigno valor y luego retorno una vez y no todo el rato (lo cambio si no te gusta)
    //Apunto ptr a la posicion que toque
    if (nblogico < DIRECTOS){
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;

    } else if (nblogico < INDIRECTOS0){
        *ptr = inodo->punterosIndirectos[0];
        return 1;

    } else if (nblogico < INDIRECTOS1){
        *ptr = inodo->punterosIndirectos[1];
        return 2;

    } else if (nblogico < INDIRECTOS2){
        *ptr = inodo->punterosIndirectos[2];
        return 3;
        
    } else {
        *ptr = 0;
        fprintf(stderr, RED
            "Error: Bloque lógico fuera de rango\n"RESET);
        return FALLO;
    }
    
    
}


int obtener_indice(unsigned int nblogico, int nivel_punteros){
    if (nblogico < DIRECTOS){
        return nblogico;

    } else if (nblogico < INDIRECTOS0){
        return nblogico - DIRECTOS;

    } else if (nblogico < INDIRECTOS1){
        if (nivel_punteros == 2){
            return (nblogico - INDIRECTOS0) / NPUNTEROS;

        } else if (nivel_punteros == 1){
            return (nblogico - INDIRECTOS0) % NPUNTEROS;

        }
    } else if (nblogico < INDIRECTOS2){
        if (nivel_punteros == 3){
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);

        } else if (nivel_punteros == 2){
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;

        } else if (nivel_punteros == 1){
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;

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
    if (nRangoBL == FALLO) return FALLO;
    nivel_punteros = nRangoBL;//el nivel_punteros +alto es el que cuelga directamente del inodo

    //iterar para cada nivel de punteros indirectos
    while (nivel_punteros > 0) {
        //no cuelgan bloques de punteros
        if (ptr == 0) {
            // bloque inexistente -> no imprimir error por pantalla!!!
            if (reservar == 0) { 
                return FALLO;
            //reservar bloques de punteros y crear enlaces desde el inodo hasta el bloque de datos
            } else {
                ptr = reservar_bloque();//de punteros
                if (ptr == FALLO) return FALLO;

                inodo->numBloquesOcupados++;
                inodo->ctime = time(NULL);//fecha actual

                //el bloque cuelga directamente del inodo
                if (nivel_punteros == nRangoBL) {
                    inodo->punterosIndirectos[nRangoBL-1] = ptr;
                    #if DEBUGN4
                        fprintf(stderr, GRAY
                            "[traducir_bloque_inodo()→ inodo.punterosIndirectos[%d] = %d (reservado BF %d para punteros_nivel%d)]\n"RESET,
                            nRangoBL-1, ptr, ptr, nivel_punteros);
                    #endif
                //el bloque cuelga de otro bloque de punteros
                } else { 
                    buffer[indice] = ptr;
                    #if DEBUGN4
                        fprintf(stderr, GRAY
                            "[traducir_bloque_inodo()→ punteros_nivel%d [%d] = %d (reservado BF %d para punteros_nivel%d)]\n"RESET,
                            nivel_punteros+1, indice, ptr, ptr, nivel_punteros);
                    #endif
                    //salvamos en el dispositivo el buffer de punteros modificado
                    if (bwrite(ptr_ant, buffer) == FALLO) return FALLO;
                }
                memset(buffer, 0, BLOCKSIZE); //ponemos a 0 todos los punteros del buffer 
            }
        //leemos del dispositivo el bloque de punteros ya existente
        } else {
            if (bread(ptr, buffer) == FALLO) return FALLO;
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        if (indice == FALLO) return FALLO;

        ptr_ant = ptr; //guardamos el puntero actual
        ptr = buffer[indice]; // y lo desplazamos al siguiente nivel 
        nivel_punteros--;
    } //al salir de este bucle ya estamos al nivel de datos

    //no existe bloque de datos
    if (ptr == 0) {
        //error lectura ∄ bloque -> no imprimir error por pantalla!!!
        if (reservar == 0) {
            return FALLO;

        } else {
            ptr = reservar_bloque();//de datos
            if (ptr == FALLO) return FALLO;

            inodo->numBloquesOcupados++;
            inodo->ctime = time(NULL);
            //si era un puntero Directo
            if (nRangoBL == 0) {
                //asignamos la direción del bl. de datos en el inodo
                inodo->punterosDirectos[nblogico] = ptr;
                #if DEBUGN4
                    fprintf(stderr, GRAY
                        "[traducir_bloque_inodo()→ inodo.punterosDirectos[%d] = %d (reservado BF %d para BL %d)]\n"RESET,
                        nblogico, ptr, ptr, nblogico);
                #endif
            } else {
                //asignamos la dirección del bloque de datos en el buffer
                buffer[indice] = ptr;
                #if DEBUGN4
                    fprintf(stderr, GRAY
                        "[traducir_bloque_inodo()→ punteros_nivel%d [%d] = %d (reservado BF %d para BL %d)]\n"RESET,
                        nivel_punteros+1, indice, ptr, ptr, nblogico);
                #endif
                 //salvamos en el dispositivo el buffer de punteros modificado
                if (bwrite(ptr_ant, buffer) == FALLO) return FALLO;
            }
        }
    }
    return ptr;
}

/*****************************************************************************************/
/*                                       NIVEL 6                                         */
/*****************************************************************************************/

int liberar_inodo(unsigned int ninodo){
    struct inodo inodo;
    struct superbloque SB;

    int bloques_liberados=0;
    //leer el inodo
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;

    //liberar todos los bloques del inodo
    bloques_liberados = liberar_bloques_inodo(0, &inodo);
    if (bloques_liberados == FALLO) return FALLO;

    //actualizar cantidad de bloques ocupados del inodo (debería quedar a 0)
    inodo.numBloquesOcupados = inodo.numBloquesOcupados - bloques_liberados;

    //marcar el inodo como tipo libre y tamEnBytesLog=0
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;

    //actualizar la lista enlazada de inodos libres
    //leer el superbloque
    if (bread(posSB, &SB) == FALLO) return FALLO;

    //incluir el inodo que queremos liberar en la lista de inodos libres
    inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;

    //incrementar la cantidad de inodos libres
    SB.cantInodosLibres++;

    // escribir el superbloque en el dispositivo virtual
    if (bwrite(posSB, &SB) == FALLO) return FALLO;

    //actualizar el ctime
    inodo.ctime = time(NULL);

    //escribir el inodo actualizado en el dispositivor virtual
    if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;

    //devolver el nº del inodo liberado
    return ninodo;
}

int liberar_bloques_inodo(unsigned int primerBL,struct inodo *inodo){
    unsigned int nivel_punteros, indice, ptr = 0, nBL, ultimoBL; 
    /*
    bloques_punteros[0] para indirectos 1 1024bytes

    leer el inodo el I0, si hay una direccion hace bread
        Y con el bread se lee el bloque de datos al que apunta I0 QUE CONTIENE PUNTEROS Y

    |SB|INODO|MB|     DATOS    |



    bloques_punteros[1] para indirectos 2 1024bytes
    leer el inodo el I1, si hay una direccion hace bread
    por ejemplo. 
        1-Comprobar que hay bloque de datos reservado
        2-leer el bloque de datos si esta reservado
        El resultado de la lectura es:

    bloques_punteros[1] = |   |116||117|        PUNTEROS       |
    bloques_punteros[1][116] = 3143

    HACE UN BRAD a bloque de datos 3143
    y lo guarda en  bloques_punteros[0] = |        PUNTEROS       |

    bloques_punteros[1][117] = 3144
    HACE UN BRAD a bloque de datos 3144
    y lo guarda en  bloques_punteros[0] = |        PUNTEROS       |





    bloques_punteros[2] para indirectos 3 1024bytes
    */
    unsigned int bloques_punteros[3][NPUNTEROS]; //array de bloques de punteros
    unsigned int bufAux_punteros[NPUNTEROS]; // para llenar de 0s y comparar
    int nRangoBL;
    int ptr_nivel[3]; //punteros a bloques de punteros de cada nivel
    int indices[3]; //indices de cada nivel
    int liberados=0; //nº de bloques liberados
    

    //Variables añadidos
    //Una copia para no modificar el original en el caso de truncar en un primerBL != 0
    unsigned int cpy_bloques_punteros[3][NPUNTEROS]; //array de bloques de punteros
    unsigned int saltado; // Para mejorar el bwrite, si ha realizado la mejora 1 ya no escribe.
    unsigned int pos;//Para obtener la posición del nBL en niveles de Indirecto.
    unsigned int oldBL; //Para imprimir cuando se ejecuta las mejoras.
    int bwriteCounter = 0,breadCounter = 0; //Contar las veces que lee o escribe bloques.
    int cpy_nRangoBL; //Para las mejoras se realiza copias del nRangoBL para no alterarlo
    int bloques;//Usado para iterar sobre el bloque de punteros (mejora 2);
    int iterador;//Usado para iterar en todas las mejoras
    int numbers[] = {1,NPUNTEROS,NPUNTEROS*NPUNTEROS}; //Usado para simplificar código (combina con casoBL)
    char casoBL = 0; //Usado para el debugging y minimizar código quita 2 while identicos (mejora 2)



    if (inodo->tamEnBytesLog == 0){
        return liberados;
    }

    //obtenemos el último bloque lógico del inodo
    if (inodo->tamEnBytesLog % BLOCKSIZE == 0){
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    } else {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }

    memset(bufAux_punteros,0,BLOCKSIZE);

    #if DEBUGN6
        fprintf(stderr, CYAN NEGRITA"[liberar_bloques_inodo()→ primer BL: %d, último BL: %d]\n"RESET, primerBL, ultimoBL);
    #endif

    for (nBL = primerBL; nBL <= ultimoBL; nBL++){
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);  //0:D, 1:I0, 2:I1, 3:I2
        if (nRangoBL < 0) return FALLO;

        nivel_punteros = nRangoBL; //el nivel_punteros +alto cuelga del inodo

        //cuelgan bloques de punteros
        while (ptr > 0 && nivel_punteros > 0) {
            indice = obtener_indice(nBL,nivel_punteros);
            if (indice == FALLO) return FALLO;

            if (indice == 0 || nBL == primerBL){
                //solo hay que leer del dispositivo si no está ya cargado previamente en un buffer
                if (bread(ptr, bloques_punteros[nivel_punteros - 1]) == FALLO) return FALLO;
                breadCounter++;
            }

            ptr_nivel[nivel_punteros - 1] = ptr;

            indices[nivel_punteros - 1] = indice;

            ptr = bloques_punteros[nivel_punteros - 1][indice];

            nivel_punteros--;
        }


        /*
        estamos en indirectos 3
        
        si llegamos a bloques_punteros[2][5] = 3146
        si llegamos a bloques_punteros[1][25] = 3147
        si llegamos a bloques_punteros[0][120] = 3148

        sale de while con ptr = 3148
        */

        //Parte de borrado
        if (ptr > 0) {
            if (liberar_bloque(ptr) == FALLO) return FALLO;
            #if DEBUGN6
                fprintf(stderr, GRAY"[liberar_bloques_inodo()→ liberado BF %d de datos para BL %d]\n"RESET, ptr, nBL);
                fflush(stderr);
            #endif
            liberados++;
            if (nRangoBL == 0) {
                inodo->punterosDirectos[nBL] = 0;

            } else {
                //Copiar el buffer de punteros para optimizar el bwrite
                //Este servira para ver si el bloque de punteros es todo 0, si lo es se escribirá
                //ya que antes está la condición de que bloques_punteros no sea todos 0
                memcpy(cpy_bloques_punteros,bloques_punteros,BLOCKSIZE*3);
                cpy_nRangoBL = nRangoBL;

                while (cpy_nRangoBL > 0) {
                    pos = obtener_indice(nBL, cpy_nRangoBL);
                    if (pos == FALLO) return FALLO;

                    //Limpia de 0-pos en la copia del buffer
                    memcpy(cpy_bloques_punteros[cpy_nRangoBL - 1], bufAux_punteros, pos * sizeof(unsigned int));
                    cpy_nRangoBL--;
                }

                nivel_punteros = 1;
                saltado = 0;
                
                while (nivel_punteros <= nRangoBL) {
                    indice = indices[nivel_punteros - 1];
                    bloques_punteros[nivel_punteros - 1][indice] = 0;
                    cpy_bloques_punteros[nivel_punteros - 1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros - 1];

                    if (memcmp(bloques_punteros[nivel_punteros - 1], bufAux_punteros, BLOCKSIZE) == 0) {
                        if(liberar_bloque(ptr) == FALLO) return FALLO;
                        #if DEBUGN6
                            fprintf(stderr, GRAY"[liberar_bloques_inodo()→ liberado BF %d de punteros_nivel%d correspondiente al BL %d]\n"RESET, ptr, nivel_punteros ,nBL);
                            fflush(stderr);
                        #endif
                        liberados++;

                        //incluir mejora 1
                        iterador = 1;//En este caso es para obtener el indice
                        cpy_nRangoBL = nRangoBL;//Para no alterar nRangoBL
                        if (cpy_nRangoBL == 3) cpy_nRangoBL--;

                        //Prohibido funcionar en punteros_nivel 3(Ya que no hay los saltos)
                        //Prohibido funcionar en el ultimo bloque logico(Si no se sale fuera de rango)
                        //Condicion de salida nRango mayor al rangoBL copiado
                        while (cpy_nRangoBL >= iterador && nBL != ultimoBL) {
                            oldBL = nBL;

                            //Para saber la posición actual en el bloque de punteros
                            pos = obtener_indice(nBL, iterador);
                            if (pos == FALLO) return FALLO;

                            if (iterador == 1) nBL += NPUNTEROS - pos - 1;
                            if (iterador == 2) nBL += (NPUNTEROS - pos - 1) * NPUNTEROS;
                                
                            if (nBL != oldBL){
                                #if DEBUGN6
                                    fprintf(stderr, GREEN"[liberar_bloques_inodo()→ Del BL %d saltamos hasta BL %d]\n"RESET,oldBL,nBL);
                                #endif
                                saltado = 1;
                            }
                            iterador++;
                        }

                        if (nivel_punteros == nRangoBL)
                            inodo->punterosIndirectos[nRangoBL-1]=0;
                        
                        nivel_punteros++;
                    } else {
                        iterador = nivel_punteros;
                        while (iterador > 0) {
                            //Si no ha realizado un salto entonces se escribe, ya que si ha saltado es que era todos 0
                            //en el array bloques_punteros.

                            //Si ya hemos borrado lo que nos interesa entonces escribir
                            if (memcmp(cpy_bloques_punteros[iterador - 1], bufAux_punteros, BLOCKSIZE) == 0 && !saltado){
                                // hemos de salir del bucle ya que no será necesario liberar los bloques de niveles
                                // superiores de los que cuelga si no está vacio
                                if (bwrite(ptr, bloques_punteros[iterador - 1]) == FALLO) return FALLO;
                                #if DEBUGN6
                                    fprintf(stderr, RED"[liberar_bloques_inodo()→ salvado BF %d de punteros_nivel%d correspondiente al BL %d]\n"RESET, ptr, nivel_punteros ,nBL);
                                #endif
                                bwriteCounter++;
                                break;
                            }
                            iterador--;
                        }
                        
                        nivel_punteros = nRangoBL + 1;
                        
                    }
                }
            }

        } else {
            //incluir mejora 2, esta es para saltar los indexados, llegar al BL ocupado en el
            if (nRangoBL > 0) {
                oldBL = nBL;
                bloques = 0;
                casoBL = 0;

                //Una copia para no modificar el original en el caso de truncar en un BL != 0
                //Copiar el buffer de punteros
                memcpy(cpy_bloques_punteros, bloques_punteros, BLOCKSIZE * 3);

                //Aqui es para limpiar buffers comprobados por la necesidad del propio algoritmo
                pos = obtener_indice(nBL, nRangoBL);
                if (pos == FALLO) return FALLO;

                //Limpia de 0-pos en la copia del buffer
                memcpy(cpy_bloques_punteros[nRangoBL - 1], bufAux_punteros, pos * sizeof(unsigned int));
                //Es modificado el algoritmo para alterar el array de indices, si es -1 significa que
                //ya esta comprobado todas.
                if (indices[1] == -1) memset(cpy_bloques_punteros[1],0,BLOCKSIZE);
                if (indices[0] == -1) memset(cpy_bloques_punteros[0],0,BLOCKSIZE);

                //Bucle para saltar bloques que son 0, diferencia entre niveles de inodo
                while(nRangoBL > 0){
                    //Utilizado para eliminación intermedio del array de punteros
                    pos = obtener_indice(nBL, nRangoBL);
                    bloques = casoBL = 0;

                    //El caso indirectos2 no es necesario
                    if (nRangoBL < 3 && pos != 0) bloques = pos;
                    
                    //Apartado de comprobación, 3 casos referentes a cada rango de indirectos
                    if (memcmp(cpy_bloques_punteros[0], bufAux_punteros, BLOCKSIZE) == 0 &&
                            memcmp(cpy_bloques_punteros[1], bufAux_punteros, BLOCKSIZE) == 0 && 
                            nRangoBL == 3){
                        casoBL = 3;
                        
                    } else if (memcmp(cpy_bloques_punteros[0], bufAux_punteros, BLOCKSIZE) == 0 && 
                             nBL == oldBL && nRangoBL == 2){
                        casoBL = 2;
                        
                    } else if (nBL == oldBL && nRangoBL == 1) 
                        casoBL = 1;

                    //Apartado de salto de bloques, incrementar nBL si 0, generico para todos los indexados
                    if (casoBL > 0){
                        while(bloques < NPUNTEROS && cpy_bloques_punteros[nRangoBL - 1][bloques] == 0){
                            bloques++;

                            nBL += numbers[nRangoBL - 1];
                        }
                        //Para decir que el nPunteros ya esta vacio, si no esta el while de arriba lo
                        //modifica igualmente
                        indices[nRangoBL - 1] = -1;
                    }

                    nRangoBL--;
                }

                //Decrementar ya que se sobrepasa 1 posicion siempre
                if (oldBL != nBL) {
                    nBL--;
                    #if DEBUGN6
                        fprintf(stderr, CYAN"[liberar_bloques_inodo()→ Del BL %d saltamos hasta BL %d]\n"RESET,oldBL,nBL);
                    #endif
                }
            }
        }
    }

    #if DEBUGN6
        //Ensenyar liberados, bread y bwrite contados.
        fprintf(stderr, CYAN NEGRITA
            "[liberar_bloques_inodo()→ total bloques liberados: %d, total_breads: %d, total_bwrites: %d]\n"RESET,
                liberados,breadCounter,bwriteCounter);
    #endif

    return liberados;
}