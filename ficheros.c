#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;
    // Ver si hay permisos de escritura
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, RED "No hay permisos de escritura\n"RESET);
        return FALLO;
    }

    //Para contar los bytes escritos
    unsigned int bytes_escritos = 0;

    // Determinar de qué bloque a qué bloque lógico hay que escribir
    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ulitmoBL = (offset + nbytes - 1) / BLOCKSIZE;

    // Determinar los desplazamientos dentro de esos bloques donde cae el offset, y los nbytes escritos a partir del offset
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    // obtener el nº de bloque físico
    unsigned int nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);
    if (nbfisico == FALLO) return FALLO;

    // leer bloque físico y almacenar en un array
    unsigned char buf_bloque[BLOCKSIZE];
    if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;

    // caso1:escritura cabe en un solo bloque
    if (primerBL == ulitmoBL) { 
        // escribir los nbytes del buf_original en la posición buf_bloque + desp1:
        memcpy(buf_bloque + desp1, buf_original, nbytes);
        // escribir buf_bloque modificado en el nº de bloque físico correspondiente
        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;
        // bytes escritos
        bytes_escritos += nbytes;

    // caso2: hay que escribir en más de un bloque
    } else { 
        // FASE 1: PRIMER BLOQUE LÓGICO
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;

        // Acumular bytes escritos.
        bytes_escritos += (BLOCKSIZE - desp1);

        // FASE 2: BLOQUES LÓGICOS INTERMEDIOS
        for (int bl = primerBL+1; bl < ulitmoBL; bl++){
            nbfisico = traducir_bloque_inodo(&inodo, bl, 1);
            if (bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE) == FALLO) return FALLO;

            // Acumular bytes escritos
            bytes_escritos += BLOCKSIZE;
        }
        // FASE 3:ÚLTIMO BLOQUE LÓGICO
        nbfisico = traducir_bloque_inodo(&inodo, ulitmoBL, 1);
        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;

        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;
        
        // Acumular bytes escritos
        bytes_escritos += (desp2+1);
    }
    // Actualizar el tamaño en bytes lógicos (si es necesario)
    if ((offset + nbytes) > inodo.tamEnBytesLog) {
        inodo.tamEnBytesLog = offset + nbytes;
    }
    // Actualizar el mtime (porque hemos escrito en la zona de datos).
    inodo.mtime = time(NULL);
    // Actualizar el ctime  (porque hemos actualizado campos del inodo).
    inodo.ctime = time(NULL);
    // Salvar el inodo con escribir_inodo().
    if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;

    return bytes_escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    if (leer_inodo(ninodo,&inodo) == FALLO) return FALLO;
    if ((inodo.permisos & 4) != 4) {
       fprintf(stderr, RED "No hay permisos de lectura\n"RESET);
       return FALLO;
    }

    unsigned int leidos = 0;
    //Comprobaciones
    if (offset >= inodo.tamEnBytesLog)
        return leidos;
    
    if((offset+nbytes) >= inodo.tamEnBytesLog)
        //Leemos solo los bytes desde el offset hasta EOF
        nbytes = inodo.tamEnBytesLog-offset;
    
    //Esto es utilizado para leer un bloque de datos
    unsigned char buf_bloque[BLOCKSIZE];

    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ulitmoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    //En traducir reservar=0, por lo que puede retornar -1
    unsigned int nbfisico = traducir_bloque_inodo(&inodo, primerBL, 0);
    
    //Mismos casos que mi_write, en vez de escribir lee
    if (primerBL == ulitmoBL) {
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original, buf_bloque + desp1, nbytes);// se ha tenido en cuenta para nbytes < BLOCKSIZE
        }
        leidos += nbytes;
    // caso2: hay que escribir en más de un bloque
    } else { 
        
        // Fase 1: leemos el primer bloque teniendo en cuenta el desplazamiento
        if (nbfisico != -1){
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1); // Guardamos una parte
        }
        leidos += BLOCKSIZE - desp1;

        // Fase 2: Un bucle hasta llegar a un bloque menor que nbytes
        for (int bl = primerBL+1; bl < ulitmoBL; bl++){
            nbfisico = traducir_bloque_inodo(&inodo, bl, 0);
            if (nbfisico != -1){
                if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
                memcpy(buf_original + leidos ,buf_bloque, BLOCKSIZE); // Guardamos una parte
            }
            // Acumular bytes escritos
            leidos += BLOCKSIZE;
        }

        // Fase 3: Ultimo bloque, tener en cuenta el desp2
        nbfisico = traducir_bloque_inodo(&inodo, ulitmoBL, 0);
        if (nbfisico != -1){
            if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
            memcpy(buf_original, buf_bloque, desp2); // Guardamos una parte
        }
        leidos += desp2 + 1;
    }
    //Actualizamos atime
    inodo.atime = time(NULL);

    if (escribir_inodo(ninodo,&inodo) == FALLO) return FALLO;
    return leidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;

    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;

    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;

    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    
    return EXITO;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;
    //Cambiamos los permisos por los introducidos por parametro
    inodo.permisos = permisos;
    //Actualizamos ctime
    inodo.ctime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;
    
    return EXITO;
}

/*****************************************************************************************/
/*                                       NIVEL 6                                         */
/*****************************************************************************************/

int mi_truncar_f(unsigned int ninodo,unsigned int nbytes){
    struct inodo inodo;
    int bloques_liberados = 0;
    int primerBL = 0;

    if (leer_inodo(ninodo, &inodo) == FALLO) return FALLO;

    // Ver si hay permisos de escritura
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, RED "No hay permisos de escritura\n"RESET);
        return FALLO;
    }

    // la función no puede truncar más allá del tamaño en bytes lógicos del inodo
    if (nbytes >= inodo.tamEnBytesLog) return bloques_liberados;

    //calcular primer bloque lógico a liberar
    if (nbytes % BLOCKSIZE == 0) {
        primerBL = nbytes / BLOCKSIZE;
    } else {
        primerBL = nbytes / BLOCKSIZE + 1;
    }
    
    bloques_liberados = liberar_bloques_inodo(primerBL, &inodo);
    if (bloques_liberados == FALLO) return FALLO;

    //Actualizar mtime y ctime
    inodo.ctime = inodo.mtime = time(NULL);

    //actualizar tamaño en bytes lógico
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados = inodo.numBloquesOcupados - bloques_liberados;

    //salvar inodo
    if (escribir_inodo(ninodo, &inodo) == FALLO) return FALLO;

    //devolver la cantidad de bloques liberados
    return bloques_liberados;
}