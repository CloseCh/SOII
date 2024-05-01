#include "directorios.h"

//Pruebas de algunos niveles

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
int leerInodo(struct superbloque bufferSB, unsigned int pos);

/*********************************************************************/
/*                              nivel 4                              */
/*********************************************************************/

int pruebaN4();
int traducirBloqueLogico(struct superbloque bufferSB);

/*********************************************************************/
/*                              nivel 5                              */
/*********************************************************************/
int pruebaN5();

/*********************************************************************/
/*                              nivel 7                              */
/*********************************************************************/
int pruebaN7();
void mostrar_buscar_entrada(char *camino, char reservar);