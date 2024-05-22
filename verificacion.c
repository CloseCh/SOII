#include "verificacion.h"

int main (int argc, char **argv){
    //Verificación de sintaxis
    if (argc < 3) {
        fprintf(stderr, RED "Sintaxis: ./verificacion <dispositivo> <directorio_simulación>\n" RESET);
        exit(FALLO);
    }

    //Declaración de variables
    struct STAT estado;
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;
    char *dispositivo = argv[1];
    char *directorio = argv[2];
    char *prueba=argv[2];
    char *ruta=argv[2];
    int num_entradas;
    int tambuffer=NUMPROCESOS*sizeof(struct entrada);
    int cant_registros_buffer_escrituras = 256; 
    //int prueba;
    int iterador;
    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    struct REGISTRO buffer_escrituras [cant_registros_buffer_escrituras];
    struct REGISTRO escritura;
    struct entrada buffer[tambuffer]; 
    struct entrada entrada;
    struct INFORMACION info;

    //Verificar que es un directorio
    if (directorio[strlen(directorio)-1] != '/'){
        fprintf(stderr, RED"No es un directorio\n"RESET);
        exit(FALLO);
    }

    //Montar dispositivo
    bmount(dispositivo);

    //Leemos el STAT del directorio
    if (mi_stat(directorio, &estado) == FALLO)
        return FALLO;
    
    num_entradas = estado.tamEnBytesLog/sizeof(struct entrada);

    //Verificar cantidad de entradas del directorio
    if (num_entradas != NUMPROCESOS){
        fprintf(stderr, RED"Fallo, no coincide la cantidad de entradas con numero de procesos\n"RESET);
        return FALLO;
    }

    //Crear el fichero "informe.txt" dentro del directorio de simulación
    strcat(directorio,"informe.txt");
    printf("%s\n", directorio);
    if(mi_creat(directorio,7)==FALLO)
        return FALLO;
    //Leer entradas del directorio de simulación
    //Leemos todo y nos ahoramos los reads del bucle (MEJORA)
    mi_read(argv[2],buffer,0,tambuffer);
    for (int i = 0; i < num_entradas; i++){
        entrada=buffer[sizeof(struct entrada)*i];
        info.pid=atoi(strchr(entrada.nombre,(int) '_'));
        sprintf(ruta, "proceso_%d/prueba.dat",info.pid);
        strcat(prueba,ruta);
        //Bucle para cada directorio de un proceso
        
        buscar_entrada(prueba,&p_inodo_dir, &p_inodo, &p_entrada, 0, 7);
        iterador=0;
        //COMPROBAR ESTE bucle
        while (mi_read(prueba, buffer_escrituras, 0, sizeof(buffer_escrituras)) > 0) {
            escritura=buffer_escrituras[iterador];
            mi_read_f(p_inodo,&escritura,0,sizeof(escritura));

            if(escritura.pid==info.pid){
                if(info.nEscrituras==0){
                    info.MayorPosicion=*buffer_escrituras;
                    info.MenorPosicion=*buffer_escrituras;
                    info.PrimeraEscritura=*buffer_escrituras;
                    info.UltimaEscritura=*buffer_escrituras;
                }else{//Comparar nº de escritura (para obtener primera y última)
                    if(buffer_escrituras->nEscritura<info.PrimeraEscritura.nEscritura)info.PrimeraEscritura=*buffer_escrituras;
                    if(buffer_escrituras->nRegistro<info.MenorPosicion.nRegistro)info.MenorPosicion=*buffer_escrituras;
                    if(buffer_escrituras->nEscritura>info.UltimaEscritura.nEscritura)info.UltimaEscritura=*buffer_escrituras;
                    if(buffer_escrituras->nRegistro>info.MayorPosicion.nRegistro)info.MayorPosicion=*buffer_escrituras;
                }
                info.nEscrituras++;       
                iterador++;
                memset(&escritura, 0, sizeof(buffer_escrituras));

            }
        //obtener la última posición de la escritura

        //Añadir la información del struct info al fichero informe.txt
        char bufferAux[TAMBUFFER]; // esto hay que cambiarlo
        memset(bufferAux, 0, TAMBUFFER);
        //SE PUEDE METER EN EL MISMO STRCAT (HECHO ASI PARA LEGILIBILIDAD)
        char texto[128];
        sprintf(texto,"PID: %d\n",info.pid);
        strcat(bufferAux,texto);
        sprintf(texto,"Numero de escrituras: %d\n",info.nEscrituras);
        strcat(bufferAux,texto);
        sprintf(texto,"Primera Escritura\t%d\t%d\t%s\n",info.PrimeraEscritura.nEscritura,info.PrimeraEscritura.nRegistro,asctime(localtime(&info.PrimeraEscritura.fecha)));
        strcat(bufferAux,texto);
        sprintf(texto,"Ultima Escritura\t%d\t%d\t%s\n",info.UltimaEscritura.nEscritura,info.UltimaEscritura.nRegistro,asctime(localtime(&info.UltimaEscritura.fecha)));
        strcat(bufferAux,texto);
        sprintf(texto,"Menor Posición\t%d\t%d\t%s\n",info.MenorPosicion.nEscritura,info.MenorPosicion.nRegistro,asctime(localtime(&info.MenorPosicion.fecha)));
        strcat(bufferAux,texto);
        sprintf(texto,"Mayor Posición\t%d\t%d\t%s\n",info.MayorPosicion.nEscritura,info.MayorPosicion.nRegistro,asctime(localtime(&info.MayorPosicion.fecha)));
        strcat(bufferAux,texto);
        

        mi_write(directorio,bufferAux,info.UltimaEscritura.nRegistro * sizeof(struct REGISTRO),sizeof(bufferAux));
        }
    }

    //Desmontar dispositivo
    bumount();

    return EXITO;
}
