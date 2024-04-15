#include "ficheros.h"

/*****************************************************************************************/
/*                                       NIVEL 7                                         */
/*****************************************************************************************/

#define ERROR_CAMINO_INCORRECTO (-2)
#define ERROR_PERMISO_LECTURA (-3)
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA (-4)
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO (-5)
#define ERROR_PERMISO_ESCRITURA (-6)
#define ERROR_ENTRADA_YA_EXISTENTE (-7)
#define ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO (-8)
#define TAMNOMBRE 60 //tamaño del nombre de directorio o fichero, en Ext2 = 256

struct entrada {
    char nombre[TAMNOMBRE];
    unsigned int ninodo;
};


/* 
 * Function: extraer_camino
 * ----------------------------
 *  Dada una cadena de caracteres camino, la separa en inicial y final.
 *  Donde inicial es hasta la segunda / y final el resto.
 *   
 *  const char *camino: cadena de caracteres introducida
 *  
 *  char *inicial: Porcion de camino entre las dos primeros /
 * 
 *  char *final: Cadena camino a partir de la segunda /
 * 
 *  char *tipo: Marca si la cadena se trata de un directorio "d" o fichero "f"
 *
 *  returns: EXITO O FALLO
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);


/* 
 * Function: buscar_entrada
 * ----------------------------
 *  Esta función nos buscará una determinada entrada (la parte *inicial del *camino_parcial 
 *  que nos devuelva extraer_camino()) entre las entradas del inodo correspondiente a su 
 *  directorio padre (identificado con *p_inodo_dir).
 *   
 *  const char *camino_parcial: cadena de caracteres
 *  
 *  unsigned int *p_inodo_dir:  nº de inodo del directorio padre 
 * 
 *  unsigned int *p_inodo: El número de inodo al que está asociado el nombre de la entrada buscada. 
 * 
 *  unsigned int *p_entrada: El número de entrada dentro del inodo *p_inodo_dir que lo contiene 
 * 
 *  char reservar: reservar=0 (llamadas desde de mi_unlink(), mi_dir(), mi_chmod(), mi_stat(),
 *  mi_read() y mi_write() de la capa de directorios) como para consultar y crear una entrada 
 *  de directorio si reservar=1, cuando ésta no exista (llamadas desde mi_creat() y mi_link() de la capa de directorios).
 * 
 *  unsigned char permisos: permisos de la entrada.
 *
 *  returns: FALLO o EXITO
 */
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, 
                    unsigned int *p_entrada, char reservar, unsigned char permisos);

void mostrar_error_buscar_entrada(int error);
