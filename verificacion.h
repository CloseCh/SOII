//verificacion.h
#include "simulacion.h"


struct INFORMACION {
    int pid;
    unsigned int nEscrituras; //validadas 
    struct REGISTRO PrimeraEscritura;
    struct REGISTRO UltimaEscritura;
    struct REGISTRO MenorPosicion;
    struct REGISTRO MayorPosicion;
};

int escribirInforme(const char *camino, struct INFORMACION info, unsigned int escrito);