#include "directorios.h"
//static struct UltimaEntrada UltimaEntradaEscritura;


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
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
    }

    return EXITO;
}

int mi_dir(const char *camino, char *buffer, char tipo, char flag) {
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    struct inodo inodo;

    //Creamos un buffer y trabajar sobre ella, necesario para ver el total
    char bufferAux[TAMBUFFER];
    memset(bufferAux, 0, TAMBUFFER);

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    
    //Buscar la entrada
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }
    
    //Variable para usar en strcat
    char lecValorInodo[128];
    memset(lecValorInodo, 0, 128);

    //Caso fichero
    
    //Caso directorio
    //Variables para leer entradas y iterar sobre ella
    unsigned int leidos = 0;
    unsigned int leidosTotal = 0;
    int cantEntrada = BLOCKSIZE/sizeof(struct entrada);

    //En el caso con flag -l, incluir lo siguiente en el bufferAux
    if (flag == 1){
        strcat(bufferAux, "Tipo\tPermisos\tmTime\t\t\tTamaño\tNombre\n");
        strcat(bufferAux, "-----------------------------------------------------------------------\n");
    }

    //Buffer de entradas
    struct entrada entradas[cantEntrada];
    memset(entradas, 0, BLOCKSIZE);

    //Imprimir total
    //Primera parte, lectura
    while ((leidos = mi_read_f(p_inodo, entradas, leidosTotal, BLOCKSIZE)) != 0){
        leidosTotal += leidos;

        //Segunda parte que itera sobre lo leido con mi_read_f
        int entradasLeidas = leidos/sizeof(struct entrada);
        int i = 0;

        //Caso sin flag de -l
        if (flag == 0){
            while(i < entradasLeidas){
                strcat(bufferAux, BLUE);
                strcat(bufferAux, entradas[i].nombre);
                strcat(bufferAux, RESET);
                strcat(bufferAux,"\t");
                i++;
            }
        
        //Caso con flag de -l
        } else {
            while(i < entradasLeidas){
                //Leer el inodo obtenido
                leer_inodo(entradas[i].ninodo, &inodo);

                //Imprimir tipo
                sprintf(lecValorInodo, "%c\t",inodo.tipo);
                strcat(bufferAux, lecValorInodo);

                //Imprimir los permisos en octal
                if (inodo.permisos & 4) strcat(bufferAux, "r"); else strcat(bufferAux, "-");
                if (inodo.permisos & 2) strcat(bufferAux, "w"); else strcat(bufferAux, "-");
                if (inodo.permisos & 1) strcat(bufferAux, "x"); else strcat(bufferAux, "-");
                strcat(bufferAux,"\t\t");

                //Imprimir mtime
                struct tm *tm; 
                tm = localtime(&inodo.mtime);
                sprintf(lecValorInodo, "%d-%02d-%02d %02d:%02d:%02d \t", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);
                strcat(bufferAux, lecValorInodo);

                //Imprimir tamaño
                sprintf(lecValorInodo, "%d\t",inodo.tamEnBytesLog);
                strcat(bufferAux, lecValorInodo);

                //Imprimir nombre
                if (inodo.tipo == 'd') sprintf(lecValorInodo, LGREEN"%s\n"RESET,entradas[i].nombre);
                if (inodo.tipo == 'f') sprintf(lecValorInodo, LBLUE"%s\n"RESET,entradas[i].nombre);
                strcat(bufferAux, lecValorInodo);

                i++;
            }
        }
        
    }

    //Para imprimir el total
    sprintf(buffer, "Total: %ld \n",leidosTotal/sizeof(struct entrada));
    strcat(buffer, bufferAux);
    return EXITO;
}



int mi_chmod(const char *camino, unsigned char permisos){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
    }
    //Si existe la entrada
    if(error == EXITO){
        mi_chmod_f(p_inodo, permisos);
        return  EXITO;
    }
    return  FALLO;
}

int mi_stat(const char *camino, struct STAT *p_stat){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6)) < 0) {
        mostrar_error_buscar_entrada(error);
    } else {
        mi_stat_f(p_inodo, p_stat);
       
        printf ("Nº de inodo: %d\n", p_inodo);
        printf ("tipo: %c\n", p_stat->tipo);
        printf ("permisos: %d\n", p_stat->permisos);
        printf ("atime: %s", ctime(&p_stat->atime));
        printf ("ctime: %s", ctime(&p_stat->ctime));
        printf ("mtime: %s", ctime(&p_stat->mtime));
        printf ("nlinks: %d\n", p_stat->nlinks);
        printf ("tamEnBytesLog: %d\n", p_stat->tamEnBytesLog);
        printf ("numBloquesOcupados: %d\n", p_stat->numBloquesOcupados);
        return EXITO;

    }
    return FALLO;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    /*mejora
    if(strcmp(UltimaEntradaEscritura.camino,camino)==0){
        p_inodo=UltimaEntradaEscritura.p_inodo;
    }else{
        //Codigo de abajo

    }
    */


    if((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    } else {
        return mi_write_f(p_inodo, buf, offset, nbytes);
    }
}

int mi_read(const char *camino,void *buf, unsigned int offset, unsigned int nbytes){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int *p_inodo = 0;
    unsigned int *p_entrada = 0;

   if( buscar_entrada(camino,&p_inodo_dir,p_inodo,p_entrada,0,6)==EXITO){
        return mi_read_f(*p_inodo,buf,offset,nbytes);
   }
   return FALLO;
}

int mi_link(const char *camino1, const char *camino2){
    struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;

    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int *p_inodo1 = 0;
    unsigned int *p_entrada = 0;
    unsigned int p_inodo_dir2 = SB.posInodoRaiz;
    unsigned int *p_inodo2 = 0;
    unsigned int *p_entrada2 = 0;
    struct inodo inodo1;
    struct inodo inodo2;

    //comprobar que la entrada camino1 exista
    if(buscar_entrada(camino1,&p_inodo_dir,p_inodo1,p_entrada,0,6)==FALLO) return FALLO;
    //comprobar que tiene permiso de lectura
    leer_inodo(*p_inodo1,&inodo1);
    if((inodo1.permisos & 4) != 4) {
        fprintf(stderr, RED "No hay permisos de lectura\n"RESET);
        return ERROR_PERMISO_LECTURA;
    }
    //comprobar que camino1 y camino2 son ficheros
    if(inodo1.tipo!='f'){
        fprintf(stderr, RED "No es un fichero\n"RESET);
        return FALLO;
    }
    //mirar que la entrada de camino2 no exista
    if(buscar_entrada(camino2,&p_inodo_dir2,p_inodo2,p_entrada2,1,6)==FALLO){
        return ERROR_ENTRADA_YA_EXISTENTE;
    }
    //Leemos la entrada creada correspondiente a camino2, o sea la entrada p_entrada2 de p_inodo_dir2
    leer_inodo(*p_inodo2,&inodo2);
    //creamos el enlace: Asociamos a esta entrada el mismo inodo que el asociado a la entrada de camino1, es decir p_inodo1.
    p_inodo2=p_inodo1;
    //Escribimos la entrada modificada en p_inodo_dir2
    escribir_inodo(p_inodo_dir2,&inodo1);
    liberar_inodo(*p_inodo2);
    inodo1.nlinks++;
    //Actualizamos ctime
    inodo1.ctime=time(NULL);
    if (escribir_inodo(*p_inodo1, &inodo1) == FALLO) return FALLO;
    return EXITO;
    
}

int mi_unlink(const char *camino){
  struct superbloque SB;
    if (bread(posSB, &SB) == FALLO) return FALLO;
    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned int *p_inodo = 0;
    unsigned int *p_entrada = 0;
    struct inodo inodo;
    //Comprobamos que esxita la entrada
    if(buscar_entrada(camino,&p_inodo_dir,p_inodo,p_entrada,0,6)==FALLO) return FALLO;
    //si es directorio no vacio salimos
    if(inodo.tipo=='d' && inodo.tamEnBytesLog >0)return FALLO;
    leer_inodo(p_inodo_dir,&inodo);
    int nentradas=inodo.tamEnBytesLog/sizeof(struct entrada);

    if(*p_entrada==nentradas-1){
        mi_truncar_f(*p_entrada,*p_inodo-nentradas);
    }else{
        //Leemos la ultima entrada
        leer_inodo(nentradas-1,&inodo);
        //La escribimos en la posicion de entrada a eliminar
        escribir_inodo(*p_entrada,&inodo);
        mi_truncar_f(*p_entrada,*p_inodo-nentradas);
    }

    leer_inodo(*p_inodo,&inodo);
    inodo.nlinks--;
    //Si no quedan enlaces se libera
     if(inodo.nlinks==0){
        liberar_inodo(*p_inodo);
     }
    inodo.ctime=time(NULL);
    if (escribir_inodo(*p_inodo, &inodo) == FALLO) return FALLO;
    return EXITO;
}