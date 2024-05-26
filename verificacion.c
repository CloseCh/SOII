#include "verificacion.h"
/* Weiyu primero queremos que sepas que eres un crack,
Segundo, nuestro código no tiene warnings :)
y tercero no funciona :(
*/    

int main (int argc, char **argv){
    //Verificación de sintaxis
    if (argc < 2) {
        fprintf(stderr, RED "Sintaxis: ./verificacion <dispositivo>\n" RESET);
        exit(FALLO);
    }

    //Declaración de variables
    struct STAT estado;
    struct entrada entrada;
    struct entrada bufferEntrada[NUMPROCESOS]; 
    struct INFORMACION info;

    char *dispositivo = argv[1];

    char directorio[256];
    memset(directorio, 0, 256);
    strcat(directorio, "/");

    char dirSimul[256];
    memset(dirSimul, 0, 256);

    char ruta[512];
    memset(ruta, 0, 512);

    unsigned int escrito = 0;
    char *pid;
    int num_entradas, iterador, tambuffer = NUMPROCESOS*sizeof(struct entrada);
    int leido = 0, total_leido = 0, registro_leido = 0;

    //Montar dispositivo
    bmount(dispositivo);

    //Verificar que es un directorio
    if (directorio[strlen(directorio)-1] != '/'){
        fprintf(stderr, RED"No es un directorio\n"RESET);
        exit(FALLO);
    }

    //Obtener la entrada de simulacion creada sobre la raiz
    if (mi_read(directorio, &entrada, 0, sizeof(struct entrada)) == FALLO)
        return FALLO;
    
    //Concatenar el nombre con el directorio creado
    strcat(dirSimul, entrada.nombre);
    strcat(directorio, dirSimul);
    strcat(directorio, "/");

    //Leemos el STAT del directorio
    if (mi_stat(directorio, &estado) == FALLO)
        return FALLO;
    
    num_entradas = estado.tamEnBytesLog/sizeof(struct entrada);

    //Verificar cantidad de entradas del directorio
    if (num_entradas != NUMPROCESOS){
        fprintf(stderr, RED"Fallo, no coincide la cantidad de entradas con numero de procesos\n"RESET);
        return FALLO;
    }

    //Leemos todo y nos ahoramos los reads del bucle (MEJORA)
    if (mi_read(directorio, bufferEntrada, 0, tambuffer) == FALLO)
        return FALLO;

    //Crear el fichero "informe.txt" dentro del directorio de simulación
    strcat(directorio,"informe.txt");
    if(mi_creat(directorio,7)==FALLO)
        return FALLO;
    
    printf("directorio: %s\n",directorio);

    //Bucle para cada directorio creados por diferentes procesos
    for (int i = 0; i < NUMPROCESOS; i++){
        //Extraer el PID a partir del nombre de la entrada y guardarlo en el registro info
        pid = strchr(bufferEntrada[i].nombre, '_');
        info.pid = atoi(pid+1); //+1 para no tener en cuenta el '_'
        info.nEscrituras = 0; //Para la comprobacion posterior
        
        //Obtener la ruta absoluta de prueba.dat
        sprintf(ruta, "/%s/%s/prueba.dat", dirSimul, bufferEntrada[i].nombre);

        int cant_registros_buffer_escrituras = 256; 
        struct REGISTRO buffer_escrituras [cant_registros_buffer_escrituras];
        memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
        
        //Empezar a leer registros
        while ((leido = mi_read(ruta, &buffer_escrituras, total_leido, sizeof(buffer_escrituras))) > 0) {
            total_leido += leido;
            iterador = 0;
            registro_leido = leido/sizeof(struct REGISTRO);
            while (iterador < registro_leido){
                if(buffer_escrituras[iterador].pid == info.pid){
                    if(info.nEscrituras == 0){
                        info.MenorPosicion = buffer_escrituras[iterador];
                        info.PrimeraEscritura = buffer_escrituras[iterador];
                        info.UltimaEscritura = buffer_escrituras[iterador];
                        info.MayorPosicion = buffer_escrituras[iterador];
                    }else{//Comparar nº de escritura (para obtener primera y última)
                        if (info.PrimeraEscritura.nEscritura > buffer_escrituras[iterador].nEscritura) info.PrimeraEscritura = buffer_escrituras[iterador];
                        else if (info.UltimaEscritura.nEscritura < buffer_escrituras[iterador].nEscritura) info.UltimaEscritura = buffer_escrituras[iterador];
                        
                        if (info.MayorPosicion.nRegistro < buffer_escrituras[iterador].nRegistro) info.MayorPosicion = buffer_escrituras[iterador];
                    }
                    info.nEscrituras++;
                }
                iterador++;
            }
            memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
        }

        //Añadir la información del struct info al fichero informe.txt
        int escritura;
        if ((escritura = escribirInforme(directorio, info, escrito)) == FALLO){
            fprintf(stderr, RED"Error de escritura en informe\n");
            return FALLO;
        }

        escrito += escritura;
        
        #if DEBUGN13
            fprintf(stderr, GRAY"[%d) %d escrituras validadas en %s]\n"RESET, i + 1, info.nEscrituras, ruta);
        #endif

        //Reiniciar variables 
        total_leido = 0;
        memset(&info, 0, sizeof(info));
    }

    //Desmontar dispositivo
    bumount();

    return EXITO;
}

int escribirInforme(const char *camino, struct INFORMACION info, unsigned int escrito){
    struct REGISTRO regAux;
    char writeInforme[1024];
    char secuencia[128];
    struct tm *time;
    memset(writeInforme, 0, 1024);
    
    memset(secuencia, 0, 128);
    sprintf(secuencia, "PID: %d\n", info.pid);
    strcat(writeInforme, secuencia);

    memset(secuencia, 0, 128);
    sprintf(secuencia, "Numero de escrituras: %d\n",info.nEscrituras);
    strcat(writeInforme, secuencia);

    regAux = info.PrimeraEscritura;
    time = localtime(&regAux.fecha);
    memset(secuencia, 0, 128);
    sprintf(secuencia, "Primera Escritura\t%d\t %d\t %d-%d-%d %02d:%02d:%02d\n",
        regAux.nEscritura, regAux.nRegistro,
        time->tm_year+1900, time->tm_mon+1,time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec);
    strcat(writeInforme, secuencia);

    regAux = info.UltimaEscritura;
    time = localtime(&regAux.fecha);
    memset(secuencia, 0, 128);
    sprintf(secuencia, "Ultima Escritura\t%d\t %d\t %d-%d-%d %02d:%02d:%02d\n",
        regAux.nEscritura, regAux.nRegistro,
        time->tm_year+1900, time->tm_mon+1,time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec);
    strcat(writeInforme, secuencia);

    regAux = info.MenorPosicion;
    time = localtime(&regAux.fecha);
    memset(secuencia, 0, 128);
    sprintf(secuencia, "Menor Posicion   \t%d\t %d\t %d-%d-%d %02d:%02d:%02d\n",
        regAux.nEscritura, regAux.nRegistro,
        time->tm_year+1900, time->tm_mon+1,time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec);
    strcat(writeInforme, secuencia);

    regAux = info.MayorPosicion;
    time = localtime(&regAux.fecha);
    memset(secuencia, 0, 128);
    sprintf(secuencia, "Mayor Posicion   \t%d\t %d\t %d-%d-%d %02d:%02d:%02d\n\n",
        regAux.nEscritura, regAux.nRegistro,
        time->tm_year+1900, time->tm_mon+1,time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec);
    strcat(writeInforme, secuencia);

    int escritura;
    if ((escritura = mi_write(camino, writeInforme, escrito, strlen(writeInforme))) < 0)
        return FALLO;
    

    return escritura;
}
