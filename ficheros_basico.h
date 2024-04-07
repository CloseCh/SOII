#include "bloques.h"
#include <time.h>
#include <limits.h>

#define posSB 0 // el superbloque se escribe en el primer bloque de nuestro FS
#define tamSB 1
#define INODOSIZE 128 // tamaño en bytes de un inodo
#define NPUNTEROS (BLOCKSIZE / sizeof(unsigned int))   // 256 punteros por bloque
#define DIRECTOS 12
#define INDIRECTOS0 (NPUNTEROS + DIRECTOS)    // 268
#define INDIRECTOS1 (NPUNTEROS * NPUNTEROS + INDIRECTOS0)    // 65.804
#define INDIRECTOS2 (NPUNTEROS * NPUNTEROS * NPUNTEROS + INDIRECTOS1) // 16.843.020

#define DEBUGN4 1

struct superbloque {
    unsigned int posPrimerBloqueMB;          // Posición absoluta del primer bloque del mapa de bits
    unsigned int posUltimoBloqueMB;          // Posición absoluta del último bloque del mapa de bits
    unsigned int posPrimerBloqueAI;          // Posición absoluta del primer bloque del array de inodos
    unsigned int posUltimoBloqueAI;          // Posición absoluta del último bloque del array de inodos
    unsigned int posPrimerBloqueDatos;       // Posición absoluta del primer bloque de datos
    unsigned int posUltimoBloqueDatos;       // Posición absoluta del último bloque de datos
    unsigned int posInodoRaiz;               // Posición del inodo del directorio raíz (relativa al AI)
    unsigned int posPrimerInodoLibre;        // Posición del primer inodo libre (relativa al AI)
    unsigned int cantBloquesLibres;          // Cantidad de bloques libres (en todo el disco)
    unsigned int cantInodosLibres;           // Cantidad de inodos libres (en el AI)
    unsigned int totBloques;                 // Cantidad total de bloques del disco
    unsigned int totInodos;                  // Cantidad total de inodos (heurística)
    char padding[BLOCKSIZE - 12 * sizeof(unsigned int)]; // Relleno para ocupar el bloque completo
};


struct inodo {     // comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
    unsigned char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
    unsigned char permisos; // Permisos (lectura y/o escritura y/o ejecución)


    /* Por cuestiones internas de alineación de estructuras, si se está utilizando
    un tamaño de palabra de 4 bytes (microprocesadores de 32 bits):
    unsigned char reservado_alineacion1 [2];
    en caso de que la palabra utilizada sea del tamaño de 8 bytes
    (microprocesadores de 64 bits): unsigned char reservado_alineacion1 [6]; */
    unsigned char reservado_alineacion1[6];


    time_t atime; // Fecha y hora del último acceso a datos
    time_t mtime; // Fecha y hora de la última modificación de datos
    time_t ctime; // Fecha y hora de la última modificación del inodo


    /* comprobar que el tamaño del tipo time_t para vuestra plataforma/compilador es 8:
    printf ("sizeof time_t is: %ld\n", sizeof(time_t)); */


    unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
    unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos (EOF)
    unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos


    unsigned int punterosDirectos[12];  // 12 punteros a bloques directos
    unsigned int punterosIndirectos[3]; /* 3 punteros a bloques indirectos:
    1 indirecto simple, 1 indirecto doble, 1 indirecto triple */


    /* Utilizar una variable de alineación si es necesario  para vuestra plataforma/compilador   */
    char padding[INODOSIZE - 2 * sizeof(unsigned char) - 3 * sizeof(time_t) - 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char)];
    // Fijarse que también se resta lo que ocupen las variables de alineación utilizadas!!!
};

/*
 * Function: tamMB
 * ----------------------------
 *  Calcula el tamaño en bloques necesario para el mapa de bits.
 *   
 *  unsigned int nbloques: cantidad de bloques.
 *
 *  returns: Tamaño del mapa de bits.
 */
int tamMB(unsigned int nbloques);

/*
 * Function: tamAI
 * ----------------------------
 *  Calcula el tamaño en bloques necesario para el array de inodos.
 *   
 *  unsigned int ninodos: cantidad de inodos
 *
 *  returns: Tamaño del array de inodos.
 */
int tamAI(unsigned int ninodos);

/*
 * Function: initSB
 * ----------------------------
 *  Inicializamos el superbloque con los valores iniciales y pasados 
 *  por parametro.
 *   
 *  unsigned int nbloques: Numero de bloque.
 *  
 *  unsigned int ninodos: 
 *
 *  returns: -1 error de lectura sobre el bloque SB, 0 si todo va bien.
 */
int initSB(unsigned int nbloques, unsigned int ninodos);

/*
 * Function: initMB
 * ----------------------------
 *  Inicializa el mapa de bits poniendo a 1 los bits que representan los 
 *  metadatos.
 *
 *  returns: -1 si error de escritura o lectura, 0 correcto todo.
 */
int initMB();

/*
 * Function: initAI
 * ----------------------------
 *  se encargará de inicializar la lista de inodos libres
 *
 *  returns: -1 si fallo de lectura o escritura de bloque, 0 si todo va bien.
 */
int initAI();

/* >
 * Function: escribir_bit
 * ----------------------------
 *  escribe el valor indicado por bit en un bit de MB
 *   
 *  unsigned int nbloque: Numero de bloque.
 * 
 *  
 *  unsigned int bit: 0(libre), 1(ocupado)
 *
 *  returns: -1 si fallo, 0 si todo va bien
 */
int escribir_bit(unsigned int nbloque, unsigned int bit);

/*
 * Function: leer_bit
 * ----------------------------
 *  Lee un bit del MB
 *   
 *  unsigned int nbloques: Numero de bloque.
 *
 *  returns: Valor del bit leido, -1 si fallo.
 */
char leer_bit(unsigned int nbloque);

/*
 * Function: reservar_bloque
 * ----------------------------
 *  Encuentra el primer bloque de MB libre y lo marca
 *
 *  returns: posicion del bloque, -1 si no encuentra o falla.
 */
int reservar_bloque();

/*
 * Function: liberar_bloque
 * ----------------------------
 *  Libera un bloque determinado escribiendo en el MB.
 *   
 *  unsigned int nbloques: Numero de bloque.
 * 
 *  returns: numero de bloque liberado, -1 si fallo.
 */
int liberar_bloque(unsigned int nbloque);

/*
 * Function: escribir_inodo
 * ----------------------------
 *  Escribe el contenido de una variable de tipo struct inodo, 
 *  pasada por referencia, en un determinado inodo del array de inodos,
 *  inodos.
 *   
 *  unsigned int ninodo: Numero de ninodo.
 *  
 *  unsigned inodo *inodo: inodo a escribir.
 * 
 *  returns: 0 si todo ha ido bien, -1 si falla.
 */
int escribir_inodo(unsigned int ninodo, struct inodo *inodo);

/*
 * Function: leer_inodo
 * ----------------------------
 *  Lee un determinado inodo del array de inodos para volcarlo 
 *  en una variable de tipo struct inodo pasada por referencia.
 *   
 *  unsigned int ninodo: Numero de ninodo.
 *  
 *  unsigned inodo *inodo: inodo al que see volcara el inodo a leer
 * 
 *  returns: 0 si todo ha ido bien, -1 si falla.
 */
int leer_inodo(unsigned int ninodo, struct inodo *inodo);

/*
 * Function: reservar_inodo
 * ----------------------------
 *  Encuentra el primer inodo libre (dato almacenado en el superbloque),
 *  lo reserva (con la ayuda de la función escribir_inodo()), 
 *  devuelve su número y actualiza la lista enlazada de inodos libres.
 *   
 *  unsigned char tipo: 'l'-libre, 'd'-directorio o 'f'-fichero
 *  
 *  unsigned char permisos: lectura y/o escritura y/o ejecución
 * 
 *  returns: la posición del inodo reservado.
 */
int reservar_inodo(unsigned char tipo, unsigned char permisos);

/*
 * Function: obtener_nRangoBL
 * ----------------------------
 *  Para obtener el rango de punteros en el que se sitúa el bloque lógico que buscamos
 *  (0:D, 1:I0, 2:I1, 3:I2), y obtenemos además la dirección almacenada en el puntero 
 *  correspondiente del inodo.
 *   
 *  struct inodo *inodo: structura para posicionar el puntero
 *  
 *  unsigned int nblogico: numero de bloque que queremos comprobar
 * 
 *  unsigned int *ptr: puntero con la dirección almacenada
 * 
 *  returns: rango de posicion de dato (0 -> 0-11, 1-> 12-267, 2-> 268-65803, 3-> 65803-16843019)
 */
int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr);

/*
 * Function: obtener_indice
 * ----------------------------
 *  Consigue el indice del bloque de punteros pasandole por parámetros el número
 *  del bloque lógico y el nivel de punteros en que se encuentra.
 *  
 *  unsigned int nblogico: número de bloque lógico que queremos comprobar 
 * 
 *  int nivel_punteros: nivel del bloque de punteros (1,2 o 3).
 * 
 *  returns: indice del bloque de punteros.
 */
int obtener_indice(unsigned int nblogico, int nivel_punteros);

/*
 * Function: traducir_bloque_inodo
 * ----------------------------
 *  Esta función se encarga de obtener el nº de bloque físico correspondiente a un 
 *  bloque lógico determinado del inodo indicado. Enmascara la gestión de los diferentes 
 *  rangos de punteros directos e indirectos del inodo, de manera que funciones externas no 
 *  tienen que preocuparse de cómo acceder a los bloques físicos apuntados desde el inodo.
 *  
 *  struct inodo *inodo: inodo que contiene el fichero o directorio.
 * 
 *  unsigned int nblogico: el bloque logico reservado
 * 
 *  unsigned char reservar: 0 para consulta, 1 para reservar.
 * 
 *  returns: el bloque logico reservado, -1 si error.
 */
int traducir_bloque_inodo(struct inodo *inodo, unsigned int nblogico, unsigned char reservar);

/*
 * Function: liberar_inodo
 * ----------------------------
 *  Liberar el inodo indicado y actualizar el campo de inodos libres del SB.
 *  
 *  unsigned int ninodo: la posicion del inodo a eliminar.
 * 
 *  returns: el bloque logico reservado, -1 si error.
 */
int liberar_inodo(unsigned int ninodo);

/**
 * 
*/
int liberar_bloques_inodo(unsigned int primerBL,struct inodo *inodo);