#include "directorios.h"


int main(int argc, char **argv){
    if (argc < 3) {
        fprintf(stderr, RED"Sintaxis: mi_cat <disco> </ruta_fichero>\n"RESET);
        exit(FALLO);
    }

    char *ruta = argv[2];   //El segundo es la ruta del fichero
    //Tras obtener la ruta comprobar que es un fichero
    if (ruta[strlen(ruta)-1] == '/'){
        fprintf(stderr, RED "La ruta debe ser un fichero\n" RESET);
        exit(FALLO);
    }

    unsigned int leidos = 0;
    unsigned int totalLeidos = 0;
    char *dispositivo = argv[1]; //Obtener el dispositivo

    unsigned int tambuffer = 1500;
    char buffer_texto[tambuffer];
    memset(buffer_texto, 0, tambuffer);

    //Montar el dispositivo
    bmount(dispositivo);

    //Realizar lecturas con mi_read
    while ((leidos = mi_read(ruta, buffer_texto, totalLeidos, tambuffer)) > 0){

        //Imprimir lo que se ha leido
        write(1,buffer_texto, leidos);
        totalLeidos += leidos;

        //Limpiar el buffer, asi se filtra la basura
        memset(buffer_texto, 0, tambuffer);
    }

    //Imprimir el total de leidos
    char string[256];
    sprintf(string, "\n\nTotal_leidos %d\n", totalLeidos);
    write(2, string, strlen(string));

    //Desmontar el dispositivo
    bumount();

    return EXITO;
}