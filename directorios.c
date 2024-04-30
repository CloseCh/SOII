#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Primero ver que ha pasado bien el directorio
    if (*camino != '/') return FALLO;

    //Inicial entre las 2 primeras barrasa, final el resto
    //Separar inicial de final y poner que tipo (si acaba en / es directorio, si no fichero)
    int cont = 1;

    //Mientras no lo encuentre o se salga
    while (camino[cont] !='/' && camino[cont] !='\0') cont++;
    
    //Si no hay segunda barra, inicial =camino
    if (camino[cont] == '\0'){
        strncpy(inicial, camino + 1, cont - 1); //Copiamos camino 
        strcpy(tipo, "f");
        strncpy(final, "", 1);
        return EXITO;
    }

    if (camino[cont] == '/'){
        strncpy(inicial, camino + 1, cont - 1); //Copiamos hasta segunda /
        strcpy(tipo, "d");
        strncpy(final, camino + cont, strlen(camino)-cont); //Copiamos hasta segunda /
        return EXITO;
    }
    return FALLO;
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, 
                    unsigned int *p_entrada, char reservar, unsigned char permisos){
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo[2];
    int cant_entradas_inodo, num_entrada_inodo;

    if(strcmp(camino_parcial, "/")==0){ // si es el directorio raíz
        struct superbloque SB;
        if (bread(posSB, &SB) == FALLO) return FALLO;

        *p_inodo = SB.posInodoRaiz; //nuestra raíz siempre estará asociada al inodo 0
        *p_entrada = 0;
        return 0;
    }
    
    //Limpiar los buffers antes de ejecutar la función, ya que si no hay basura en los buffers
    memset(inicial, 0, sizeof(inicial));
    memset(final, 0, sizeof(final));
    if (extraer_camino(camino_parcial, inicial, final, tipo) == FALLO) return ERROR_CAMINO_INCORRECTO;
    #if DEBUGN7
        fprintf(stderr, 
            GRAY"[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n"RESET,
            inicial,final,reservar);
        fflush(stderr);
    #endif

    //buscamos la entrada cuyo nombre se encuentra en inicial
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == FALLO) return FALLO;

    if ((inodo_dir.permisos & 4) != 4)  return ERROR_PERMISO_LECTURA;

    //inicializar el buffer de lectura con 0s
    struct entrada bufferEntrada[BLOCKSIZE/sizeof(struct entrada)];
    memset(bufferEntrada, 0, BLOCKSIZE);

    //calcular cant_entradas_inodo
    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    num_entrada_inodo = 0;
    int iterador = 0;
    if (cant_entradas_inodo > 0) {
        if (mi_read_f(*p_inodo_dir, bufferEntrada, num_entrada_inodo * sizeof(struct entrada), BLOCKSIZE) == FALLO) return FALLO;
        while (num_entrada_inodo <= cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0) {
            entrada = bufferEntrada[iterador];
            if (iterador == BLOCKSIZE/sizeof(struct entrada)) iterador = 0;
            if (iterador == 0)
                if (mi_read_f(*p_inodo_dir, bufferEntrada, num_entrada_inodo * sizeof(struct entrada), BLOCKSIZE) == FALLO) return FALLO;
            
            num_entrada_inodo++;
            iterador++;
        }
        num_entrada_inodo--;
    }

    if (strcmp(inicial, entrada.nombre) != 0 && num_entrada_inodo == cant_entradas_inodo){
        //la entrada no existe
        switch (reservar) {
        case 0: // modo consulta. Como no existe retornamos error
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1: // modo escritura
            // Creamos la entrada en el directorio referenciado por *p_inodo_dir
            // si es fichero no permite escritura
            if (inodo_dir.tipo == 'f') return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;

            //si es directorio comprobar que tiene permiso de escritura
            if ((inodo_dir.permisos & 2) != 2) { 
                return ERROR_PERMISO_ESCRITURA;

            } else {
                //copiar *inicial en el nombre de la entrada
                strcpy(entrada.nombre, inicial);
                if (strcmp(tipo, "d") == 0) {
                    if (strcmp(final, "/") == 0) {
                        entrada.ninodo = reservar_inodo('d', permisos);

                        if (entrada.ninodo == FALLO) return FALLO;
                        #if DEBUGN7
                            fprintf(stderr, GRAY
                                "[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n"RESET, 
                                entrada.ninodo, 'd', permisos, entrada.nombre);
                            fflush(stderr);
                        #endif

                    // no es el final de la ruta
                    } else return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        
                // es un fichero
                } else {
                    entrada.ninodo = reservar_inodo('f', permisos);

                    #if DEBUGN7
                        fprintf(stderr, GRAY
                            "[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n"RESET, 
                            entrada.ninodo, 'f', permisos, entrada.nombre);
                        fflush(stderr);
                    #endif
                }
                
                //Escribir en la entrada del directorio padre
                inodo_dir.tamEnBytesLog += sizeof(struct entrada);
                if(mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog, sizeof(struct entrada)) == FALLO){
                    if(entrada.ninodo != -1) liberar_inodo(entrada.ninodo);
                    return FALLO;
                }
                #if DEBUGN7
                    fprintf(stderr, GRAY
                        "[buscar_entrada()→ creada entrada: %s, %d]\n"RESET, 
                        entrada.nombre, entrada.ninodo);
                    fflush(stderr);
                #endif
                
                return EXITO;
            }
        }
    }

    //La recursividad se acaba si final == "/" o ""
    if ((strcmp(final, "/") == 0) || (strcmp(final, "") == 0)) {
        if(num_entrada_inodo < cant_entradas_inodo && reservar == 1){
            //modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        //cortamos la recusividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return EXITO;
    } else {
        p_inodo_dir = &entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }

    return EXITO;
}



void mostrar_error_buscar_entrada(int error) {
    // fprintf(stderr, "Error: %d\n", error);
    switch (error) {
        case -2: fprintf(stderr, RED"Error: Camino incorrecto.\n" RESET); break;
        case -3: fprintf(stderr, RED"Error: Permiso denegado de lectura.\n"RESET); break;
        case -4: fprintf(stderr, RED"Error: No existe el archivo o el directorio.\n"RESET); break;
        case -5: fprintf(stderr, RED"Error: No existe algún directorio intermedio.\n"RESET); break;
        case -6: fprintf(stderr, RED"Error: Permiso denegado de escritura.\n"RESET); break;
        case -7: fprintf(stderr, RED"Error: El archivo ya existe.\n"RESET); break;
        case -8: fprintf(stderr, RED"Error: No es un directorio.\n"RESET); break;
    }
}

int mi_creat(const char *camino, unsigned char permisos){
    unsigned int *p_inodo;
    unsigned int *p_entrada;

    buscar_entrada(camino,0,p_inodo,p_entrada,1,permisos);
}

int mi_dir(const char *camino, char *buffer){
    struct inodo inodo;
    unsigned int *p_inodo;
    unsigned int *p_entrada;

    buscar_entrada(camino,1,p_inodo,p_entrada,0,6/*no se que poner*/);
    leer_inodo(*p_inodo,&inodo);

    if(inodo.tipo !='d') return FALLO;
    if((inodo.permisos & 4) != 4) {
       fprintf(stderr, RED "No hay permisos de lectura\n"RESET);
       return ERROR_PERMISO_LECTURA;
    }
    //CONTINUARÁ

}

int mi_chmod(const char *camino, unsigned char permisos){

    unsigned int *p_inodo;
    unsigned int *p_entrada;

    p_inodo=buscar_entrada(camino,0,p_inodo,p_entrada,1,permisos);
    //Si existe la entrada
    if(p_inodo==EXITO){
        mi_chmod_f(p_inodo,permisos);
        return  EXITO;
    }
    return  FALLO;

}

int mi_stat(const char *camino, struct STAT *p_stat){

     unsigned int *p_inodo;
     unsigned int *p_entrada;
     
    if(buscar_entrada(camino,1,p_inodo,p_entrada,0,p_stat->permisos)==EXITO){
        mi_stat_f(p_inodo,p_stat);
       
        printf ("Nº de inodo: %d\n", *p_inodo);
        printf ("tipo: %c\n", p_stat->tipo);
        printf ("permisos: %c\n", p_stat->permisos);
        printf ("atime: %s\n", p_stat->atime);
        printf ("ctime: %s\n", p_stat->ctime);
        printf ("mtime: %s\n", p_stat->mtime);
        printf ("nlinks: %s\n", p_stat->nlinks);
        printf ("tamEnBytesLog: %d\n", p_stat->tamEnBytesLog);
        printf ("numBloquesOcupados: %d\n", p_stat->numBloquesOcupados);
        return EXITO;

    }
    return FALLO;
}