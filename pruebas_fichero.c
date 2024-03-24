#include "pruebas_fichero.h"

int pruebaN2(){
    struct superbloque bufferSB;
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }

    //Ver dato del superbloque
    lecturaSB(bufferSB);
    printf ("\n");

    //Datos de inodo
    lecturaInodo(bufferSB);
    printf("\n");

    return EXITO;
}

int lecturaSB(struct superbloque bufferSB){
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

    printf ("sizeof struct superbloque: %ld\n", sizeof(bufferSB));

    return EXITO;
}

//Lectura de inodo en el nivel 2
int lecturaInodo(struct superbloque bufferSB){
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
    printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES");
    unsigned int i = 1;
    unsigned int cantidadAI = bufferSB.posUltimoBloqueAI - bufferSB.posPrimerBloqueAI + 1;
    while(i <= cantidadAI){
        for(int j = 0; j < BLOCKSIZE/INODOSIZE; j++){
            printf("%d ",bufferAI[j].punterosDirectos[0]);
        }
        //Leer el siguiente bloque AI
        if(bread(bufferSB.posPrimerBloqueAI + i, &bufferAI) == FALLO){
            fprintf(stderr, RED
                "Error: lectura AI\n"RESET);
            return FALLO;
        }
        i++;
    }
    printf ("\n");

    return EXITO;
}

int pruebaN3(){
    struct superbloque bufferSB;
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }

    //Ver dato del superbloque
    lecturaSB(bufferSB);
    printf ("\n");

    //Prueba de reservar y liberar un bloque
    reservaProf(bufferSB);
    printf ("\n");

    //Leer bits
    leerBit(bufferSB);
    printf ("\n");

    //Leer directorio raiz reservado
    printf ("DATOS DEL DIRECTORIO RAIZ\n");
    leerInodo(bufferSB,0);
    printf ("\n");

    printf ("\n");
    return EXITO;
}

int leerBit(struct superbloque bufferSB){
    //Leer bits
    printf ("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
    unsigned int bit = leer_bit(posSB);
    printf ("posSB: %d → leer_bit(%d) = %d\n", posSB, posSB, bit);

    bit = leer_bit(bufferSB.posPrimerBloqueMB);
    printf ("SB.posPrimerBloqueMB: %d → leer_bit(%d) = %d\n", 
        bufferSB.posPrimerBloqueMB, bufferSB.posPrimerBloqueMB, bit);

    bit = leer_bit(bufferSB.posUltimoBloqueMB);
    printf ("SB.posUltimoBloqueMB: %d → leer_bit(%d) = %d\n", 
        bufferSB.posUltimoBloqueMB, bufferSB.posUltimoBloqueMB, bit);
    
    bit = leer_bit(bufferSB.posPrimerBloqueAI);
    printf ("SB.posPrimerBloqueAI: %d → leer_bit(%d) = %d\n", 
        bufferSB.posPrimerBloqueAI, bufferSB.posPrimerBloqueAI, bit);
    
    bit = leer_bit(bufferSB.posUltimoBloqueAI);
    printf ("SB.posUltimoBloqueAI: %d → leer_bit(%d) = %d\n", 
        bufferSB.posUltimoBloqueAI, bufferSB.posUltimoBloqueAI, bit);
    
    bit = leer_bit(bufferSB.posPrimerBloqueDatos);
    printf ("SB.posPrimerBloqueDatos: %d → leer_bit(%d) = %d\n", 
        bufferSB.posPrimerBloqueDatos, bufferSB.posPrimerBloqueDatos, bit);

    bit = leer_bit(bufferSB.posUltimoBloqueDatos);
    printf ("SB.posUltimoBloqueDatos: %d → leer_bit(%d) = %d\n", 
        bufferSB.posUltimoBloqueDatos, bufferSB.posUltimoBloqueDatos, bit);

    return EXITO;
}

int reservaProf(struct superbloque bufferSB){
    //Prueba de reservar y liberar un bloque
    unsigned int reservarB = reservar_bloque();
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }
    printf ("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS \n");
    printf ("Se ha reservado el bloque físico nº %d que era el 1º libre indicado por el MB\n", reservarB);
    printf ("SB.cantBloquesLibres = %d \n", bufferSB.cantBloquesLibres);
    liberar_bloque(reservarB);
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }
    printf ("Liberamos ese bloque y después SB.cantBloquesLibres = %d\n", bufferSB.cantBloquesLibres);

    return EXITO;
}

int leerInodo(struct superbloque bufferSB, unsigned int pos){
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    //Lectura del inodos
    if(bread(bufferSB.posPrimerBloqueAI, inodos) == FALLO){
        fprintf(stderr, RED
            "Error: lectura inodo en leerRaiz\n"RESET);
        return FALLO;
    }

    printf ("tipo: %c\n", inodos[pos].tipo);
    printf ("permisos: %d\n", inodos[pos].permisos);
    printf ("atime: %s", ctime(&inodos[pos].atime));
    printf ("ctime: %s", ctime(&inodos[pos].ctime));
    printf ("mtime: %s", ctime(&inodos[pos].mtime));
    printf ("nlinks: %d\n", inodos[pos].nlinks);   
    printf ("tamEnBytesLog: %d\n", inodos[pos].tamEnBytesLog);
    printf ("numBloquesOcupados: %d\n", inodos[pos].numBloquesOcupados);  


    return EXITO;
}

int pruebaN4(){
    struct superbloque bufferSB;
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }

    //Ver dato del superbloque
    lecturaSB(bufferSB);
    printf ("\n");

    //Traducción de bloques logicos
    printf ("INODO 1. TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 468.750\n\n");
    traducirBloqueLogico(bufferSB);
    printf ("\n");

    //DATOS D(EL INODO RESERVADO 1
    printf ("DATOS DEL DIRECTORIO RAIZ\n");
    leerInodo(bufferSB,1);
    printf ("\n");

    //Actualizar el buffer
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }
    //Ver inodos libres
    printf ("posPrimerInodoLibre = %i \n", bufferSB.posPrimerInodoLibre);
    printf ("\n");
    return EXITO;
}

int traducirBloqueLogico(struct superbloque bufferSB){
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    reservar_inodo('f',6);
    
    //Lectura del inodos
    if(bread(bufferSB.posPrimerBloqueAI, inodos) == FALLO){
        fprintf(stderr, RED
            "Error: lectura inodo en leerRaiz\n"RESET);
        return FALLO;
    }
    
    traducir_bloque_inodo(&inodos[1], 8, 1);
    traducir_bloque_inodo(&inodos[1], 204, 1);
    traducir_bloque_inodo(&inodos[1], 30004, 1);
    traducir_bloque_inodo(&inodos[1], 400004, 1);
    traducir_bloque_inodo(&inodos[1], 468750, 1);

    if(bwrite(bufferSB.posPrimerBloqueAI, inodos) == FALLO){
        fprintf(stderr, RED
            "Error: lectura inodo en leerRaiz\n"RESET);
        return FALLO;
    }

    return EXITO;
}

int pruebaN5(){
    struct superbloque bufferSB;
    if(bread(posSB, &bufferSB) == FALLO){
        fprintf(stderr, RED
            "Error: lectura SB\n"RESET);
        return FALLO;
    }

    //Ver dato del superbloque
    lecturaSB(bufferSB);
    printf ("\n");

    return EXITO;
}