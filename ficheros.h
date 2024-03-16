#include "ficheros_basico.h"

struct STAT {     // comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
    unsigned char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
    unsigned char permisos; // Permisos (lectura y/o escritura y/o ejecución)

    time_t atime; // Fecha y hora del último acceso a datos
    time_t mtime; // Fecha y hora de la última modificación de datos
    time_t ctime; // Fecha y hora de la última modificación del inodo

    unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
    unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos (EOF)
    unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos+
};

/*
 * Function: mi_write_f
 * ----------------------------
 * Escribe el contenido procedente de un buffer de memoria, buf_original, de tamaño nbytes,
 * en un fichero/directorio (correspondiente al inodo pasado como argumento, ninodo): le indicamos 
 * la posición de escritura inicial en bytes lógicos, offset, con respecto al inodo, y el número 
 * de bytes, nbytes, que hay que escribir. 
 *   
 *  unsigned int ninodo: ---
 * 
 *  const void *buf_original: buffer que se desea escribir
 * 
 *  unsigned int offset: posición inicial en bytes lógicos
 * 
 *  unsigned int nbytes: número de bytes a escribir
 *
 *  returns: la cantidad de bytes escritos
 */
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);

/*
 * Function: mi_read_f
 * ----------------------------
 * ---
 *   
 *  unsigned int ninodo: ---
 * 
 *  const void *buf_original: ---
 * 
 *  unsigned int offset: ---
 * 
 *  unsigned int nbytes: ---
 *
 *  returns: ---
 */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);


/*
 * Function: mi_stat_f
 * ----------------------------
 *  Devuelve la metainformación de un fichero/directorio (correspondiente al nº de inodo pasado 
 *  como argumento): tipo, permisos, cantidad de enlaces de entradas en directorio, tamaño en 
 *  bytes lógicos, timestamps y cantidad de bloques ocupados en la zona de datos, es decir 
 *  todos los campos menos los punteros.
 *   
 *  unsigned int ninodo: numero de inodo
 * 
 *  struct STAT *p_stat: struct STAT (igual que inodo pero sin punteros)
 *
 *  returns: ---
 */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);

/*
 * Function: mi_chmod_f
 * ----------------------------
 *  ---
 *   
 *  unsigned int ninodo: ---
 * 
 *  unsigned char permisos: ---
 *
 *  returns: ---
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);