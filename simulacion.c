#include "simulacion.h"

static unsigned int acabados;

int main(int argc, char **argv){
    //Comprobación de sintaxis
    if (argc < 2){
        fprintf(stderr, RED"Uso: ./simulacion <disco>");
        exit(FALLO);
    }

    //Relacionar la señal SIGCHLD a reaper
    signal(SIGCHLD, reaper);

    //Montar el dispositivo padre
    char *dispositivo = argv[1];
    bmount(dispositivo);

    //Crear el directorio : /simul_aaaammddhhmmss/
    // variables para almacenar los componentes de fecha y hora
    int hours, minutes, seconds, day, month, year;
    time_t now; // Obtener la hora actual
    time(&now);

    struct tm *local = localtime(&now);

    hours = local->tm_hour;         // obtener horas desde la medianoche (0-23)
    minutes = local->tm_min;        // obtener minutos pasados después de la hora (0-59)
    seconds = local->tm_sec;        // obtener segundos pasados después de un minuto (0-59)
 
    day = local->tm_mday;            // obtener el día del mes (1 a 31)
    month = local->tm_mon + 1;      // obtener el mes del año (0 a 11)
    year = local->tm_year + 1900;   // obtener el año desde 1900


    char directorio[512] = "";
    sprintf(directorio, "/simul_%d%02d%02d%02d%02d%02d/", year, month, day, hours, minutes, seconds);
    if (mi_creat(directorio, 6) == FALLO){
        fprintf(stderr, RED"Error: fallo al crear directorio. \n"RESET);
        bumount();
        exit(0);
    }

    //Inicializamos acabados a 0;
    acabados = 0;

    for (int proceso = 1; proceso <= NUMPROCESOS; proceso++){
        int pid = fork();
        if (pid == 0){
            // Montar el dispositivo.
            bmount(dispositivo);

            //Crear el directorio del proceso hijo añadiendo el PID al nombre.
            char ruta[64] = "";
            sprintf(ruta, "proceso_%d/",getpid());
            strcat(directorio, ruta);

            if (mi_creat(directorio, 6) == FALLO){
                fprintf(stderr, RED"Error: fallo al crear directorio. \n"RESET);
                bumount();
                exit(0);
            }

            //Crear el fichero prueba.dat dentro del directorio anterior.
            strcat(directorio, "prueba.dat");
            if (mi_creat(directorio, 6) == FALLO){
                fprintf(stderr, RED"Error: fallo al crear el fichero. \n"RESET);
                fprintf(stderr, RED"Con ruta: %s. \n"RESET, directorio);
                bumount();
                exit(0);
            }

            // Inicializar la semilla de números aleatorios
            srand(time(NULL) + getpid());

            int i;
            for (i = 0; i < NUMESCRITURAS; i++){
                //Inicilizar registro
                struct REGISTRO registro;

                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = i+1;
                registro.nRegistro = rand() % REGMAX;

                //Escribir en el registro
                if (mi_write(directorio, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO)) == FALLO){
                    fprintf(stderr, RED"Error: fallo de escritura. \n"RESET);
                    fprintf(stderr, RED"Con ruta: %s. \n"RESET, directorio);
                    bumount();
                    exit(0);
                }

                #if DEBUGN121
                    fprintf(stderr, GRAY"[simulación.c → Escritura %d en %s]\n"RESET, i, directorio);
                #endif 

                //Esperar 0.05 seg para la siguiente escritura
                usleep(50000);
            }

            #if DEBUGN122
                fprintf(stderr, GRAY"[Proceso %d: Completadas %d escrituras en %s]\n"RESET, proceso, i, directorio);
            #endif 
            //Desmontar el dispositivo
            bumount();
            exit(0);//Necesario para que se emita la señal SIGCHLD
        }
        
        //Esperar 0,15 seg para lanzar siguiente proceso.
        usleep(150000);
    }

    //Permitir que el padre espere por todos los hijos
    while (acabados < NUMPROCESOS){
        pause();
    }

    //Desmontar el dispositivo
    bumount();
    exit(0);
}

void reaper(){
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
        acabados++;
    }
    //Para probar cual ha acabado
    #if DEBUGN121
        fprintf(stderr, GRAY"Acabado: %d \n"RESET, acabados);
    #endif
}
