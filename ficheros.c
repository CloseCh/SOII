#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    // Ver si hay permisos de escritura
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, RED "No hay permisos de escritura\n"RESET);
        return FALLO;
    }

    //Para contar los bytes escritos
    unsigned int bytes_escritos = 0;

    // Determinar de qué bloque a qué bloque lógico hay que escribir
    int primerBL = offset / BLOCKSIZE;
    int ulitmoBL = (offset + nbytes - 1) / BLOCKSIZE;

    // Determinar los desplazamientos dentro de esos bloques donde cae el offset, y los nbytes escritos a partir del offset
    int desp1 = offset % BLOCKSIZE;
    int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    // obtener el nº de bloque físico
    int nbfisico = traducir_bloque_inodo(&inodo, primerBL, 1);
    if (nbfisico == FALLO) return FALLO;

    // leer bloque físico y almacenar en un array
    unsigned char buf_bloque[BLOCKSIZE];
    if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;

    // caso1:escritura cabe en un solo bloque
    if (primerBL == ulitmoBL) { 
        // escribir los nbytes del buf_original en la posición buf_bloque + desp1:
        memcpy(buf_bloque + desp1, buf_original, nbytes);
        // escribir buf_bloque modificado en el nº de bloque físico correspondiente
        if(bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;
        // bytes escritos
        bytes_escritos += nbytes;

    // caso2: hay que escribir en más de un bloque
    } else { 
        // FASE 1: PRIMER BLOQUE LÓGICO
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bwrite(nbfisico, buf_bloque) == FALLO) return FALLO;

        // Acumular bytes escritos.
        bytes_escritos += (BLOCKSIZE-desp1);

        // FASE 2: BLOQUES LÓGICOS INTERMEDIOS
        for (int bl = primerBL+1; bl < ulitmoBL; bl++){
            nbfisico = traducir_bloque_inodo(&inodo, bl, 1);
            if (bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE) == FALLO) return FALLO;
            // Acumular bytes escritos
            bytes_escritos += BLOCKSIZE;
        }
        // FASE 3:ÚLTIMO BLOQUE LÓGICO
        nbfisico = traducir_bloque_inodo(&inodo, ulitmoBL, 1);
        bread(nbfisico, buf_bloque);

        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
        bwrite(nbfisico, buf_bloque);
        // Acumular bytes escritos
        bytes_escritos += (desp2+1);
    }
    // Actualizar el tamaño en bytes lógicos (si es necesario)
    if((offset+nbytes) > inodo.tamEnBytesLog){
        inodo.tamEnBytesLog = offset+nbytes;
    }
    // Actualizar el mtime (porque hemos escrito en la zona de datos).
    inodo.mtime = time(NULL);
    // Actualizar el ctime  (porque hemos actualizado campos del inodo).
    inodo.ctime = time(NULL);
    // Salvar el inodo con escribir_inodo().
    escribir_inodo(ninodo, &inodo);

    return bytes_escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 4) != 4) {
       fprintf(stderr, RED "No hay permisos de lectura\n"RESET);
       return FALLO;
    }

    int leidos = 0;
    if (offset >= inodo.tamEnBytesLog)
        return leidos;
    
    if((offset+nbytes) >= inodo.tamEnBytesLog)
        //Leemos solo los bytes desde el offset hasta EOF
        nbytes = inodo.tamEnBytesLog-offset;
        
    
    int primerBL = offset / BLOCKSIZE;
    int ulitmoBL = (offset + nbytes - 1) / BLOCKSIZE;
    //En traducir reservar=0, por lo que puede retornar -1
    int nbfisico = traducir_bloque_inodo(&inodo, primerBL, 0);
    int desp1 = offset % BLOCKSIZE;
    int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE];
    //Si no hay bloque fisico no hay que hacer el bread ni memcpy
    //Pero si que hay que sumar los bytes leidos
    if (nbfisico != -1) {
        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
        memcpy(buf_bloque + desp1, buf_original, nbytes);
    } else leidos += BLOCKSIZE;
    
    //Mismos casos que mi_write, en vez de escribir lee
    if(primerBL == ulitmoBL){
        memcpy(buf_bloque + desp1, buf_original, nbytes);
        // escribir buf_bloque modificado en el nº de bloque físico correspondiente
        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
        // bytes escritos
        leidos += nbytes;

    // caso2: hay que escribir en más de un bloque
    } else { 
        // FASE 1: PRIMER BLOQUE LÓGICO
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bread(nbfisico, buf_bloque) == FALLO) return FALLO;
        // Acumular bytes escritos.
        leidos += (BLOCKSIZE-desp1);
        // FASE 2: BLOQUES LÓGICOS INTERMEDIOS
        for (int bl = primerBL+1; bl < ulitmoBL; bl++){
            nbfisico = traducir_bloque_inodo(&inodo, bl, 0);
            if (bread(nbfisico, buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE) == FALLO) return FALLO;
            // Acumular bytes escritos
            leidos += BLOCKSIZE;
        }
        // FASE 3:ÚLTIMO BLOQUE LÓGICO
        nbfisico = traducir_bloque_inodo(&inodo, ulitmoBL, 0);
        bread(nbfisico, buf_bloque);

        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
        bread(nbfisico, buf_bloque);
        // Acumular bytes escritos
        leidos += (desp2+1);
    }
    //Actualizamos atime
    inodo.atime = time(NULL);

    return leidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

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

int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    //Cambiamos los permisos por los introducidos por parametro
    inodo.permisos=permisos;
    //Actualizamos ctime
    inodo.ctime= time(NULL);
    return EXITO;
}
