#include "pruebas_fichero.h"

int main(int argc, char **argv){
    //montar el dispositivo virtual
    bmount(argv[1]);

    unsigned int nivel = 0;

    if (argc > 2)
    nivel = atoi(argv[2]);

    switch (nivel) {
        case 2: pruebaN2(); break;
        case 3: pruebaN3(); break;
        case 4: pruebaN4(); break;
        case 5: pruebaN5(); break;
        case 7: pruebaN7(); break;
        default: pruebaN0(); break;
    }
    printf ("\n");

    //desmontar el dispositivo virtual
    bumount();
}
