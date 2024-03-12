#include "pruebas_fichero.h"

int main(int argc, char **argv){
    /****************************************************************************/
    /*                                nivel 2                                   */
    /****************************************************************************/
    //montar el dispositivo virtual
    bmount(argv[1]);

    unsigned int nivel = atoi(argv[2]);

    switch (nivel)
    {
    case 2:
        pruebaN2();
        break;
    case 3:
        pruebaN3();
        break;
    default:
        printf (RED"Nivel no implementada.\n"RESET);
        break;
    }
    printf ("\n");

    //Leer inodo
    printf ("DATOS DEL DIRECTORIO RAIZ \n");

    //desmontar el dispositivo virtual
    bumount();
}
