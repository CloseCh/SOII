#include "ficheros_basico.h"

//Pruebas de cada nivel

/*********************************************************************/
/*                              nivel 2                              */
/*********************************************************************/

int pruebaN2();
int lecturaSB(struct superbloque bufferSB);
int lecturaInodo(struct superbloque bufferSB);

/*********************************************************************/
/*                              nivel 3                              */
/*********************************************************************/

int pruebaN3();
int leerBit(struct superbloque bufferSB);
int reservaProf(struct superbloque bufferSB);
int leerRaiz(struct superbloque bufferSB);