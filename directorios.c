#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Inicial entre las 2 primeras barrasa, final el resto
    //Separar inicial de final y poner que tipo (si acaba en / es directorio, si no fichero)
    int cont=1;
    int ch='/';
    char *aux =camino+1; //Quitamos la primera barra
    //Mientras no lo encuentre o se salga
    while(camino[cont]!='/' && camino[cont]!='\0'){
        cont++;
        
    }
    //Si no hay segunda barra, inicial =camino
    if(camino[cont]=='\0'){
        inicial=aux; //Copiamos camino 
        tipo="f";
        final="";
        return EXITO;
    }

    if(camino[cont]=='/'){
        strncpy(inicial,aux,cont-1); //Copiamos hasta segunda /
        tipo="d";
        final=strchr(aux,ch); //Copiamos a partir de segunda /
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
    char tipo;
    int cant_entradas_inodo,num_entrada_inodo;

    if(strcmp(camino_parcial,"/")==0){ // si es el directorio raíz
        struct superbloque SB;
        bread(posSB, &SB);
        *p_inodo=SB.posInodoRaiz; //nuestra raíz siempre estará asociada al inodo 0
        *p_entrada=0;
        return 0;
    }

    
    if(extraer_camino(camino_parcial,inicial,final,&tipo)==FALLO) return ERROR_CAMINO_INCORRECTO;

    //buscamos la entrada cuyo nombre se encuentra en inicial
    leer_inodo(*p_inodo_dir,&inodo_dir);
    if ((inodo_dir.permisos & 4) != 4) {
       fprintf(stderr, RED "No hay permisos de lectura\n"RESET);
       return ERROR_PERMISO_LECTURA;
    }
    //inicializar el buffer de lectura con 0s
    struct entrada buffer[BLOCKSIZE/sizeof(struct entrada)];
    memset(buffer,0,BLOCKSIZE); 
    //calcular cant_entradas_inodo
    cant_entradas_inodo=inodo_dir.nlinks; // !! COMPROBAR, PUEDE ESTAR MAL
    num_entrada_inodo=0;
    if(cant_entradas_inodo>0){
        //leer entrada
        mi_read_f(*p_inodo_dir,buffer,num_entrada_inodo,sizeof(entrada));
        while((num_entrada_inodo<cant_entradas_inodo)&&(inicial!=entrada.nombre)){
            num_entrada_inodo++;
            //inicializar el buffer de lectura con 0s
            memset(buffer,0,sizeof(struct entrada));
            //leer siguiente entrada
            mi_read_f(*p_inodo_dir,buffer,num_entrada_inodo,sizeof(entrada));
        }
    }

    if((inicial!=entrada.nombre)&&(num_entrada_inodo==cant_entradas_inodo)){
        //la entrada no existe
        switch (reservar)
        {
        case 0: // modo consulta. Como no existe retornamos error
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1: // creamos la entrada en el directorio referenciado por *p_inodo_dir
            if(inodo_dir.tipo=='f'){ // si es fichero no permite escritura
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            if((inodo_dir.permisos & 2) != 2){ //si es directorio comprobar que tiene permiso de escritura
                fprintf(stderr, RED "No hay permisos de escritura\n"RESET);
                return ERROR_PERMISO_ESCRITURA;
            }else{
                //copiar *inicial en el nombre de la entrada
                strcpy(entrada.nombre,inicial);
                if(tipo=='d'){
                    if(strcmp(final,"/")==0){
                        entrada.ninodo=reservar_inodo('d',permisos);
                    }else{ // no es el final de la ruta
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }else{ // es un fichero
                    entrada.ninodo=reservar_inodo('f',permisos);
                }
        
                if( escribir_inodo(p_entrada,p_inodo_dir)==FALLO){ // es girado?
                    if(entrada.ninodo!=-1){
                        liberar_inodo(entrada.ninodo);
                    }
                    return FALLO;
                }
            }
        }
    }

    if((strcmp(final,"/")==0)||(strcmp(final,"")==0)){ // hemos llegado al final del camino, se refiere a esto??
        if((num_entrada_inodo<cant_entradas_inodo)&&(reservar==1)){
            //modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        //cortamos la recusividad
         *p_inodo=entrada.ninodo;
         *p_entrada=num_entrada_inodo;
        return EXITO;
    }else{
        *p_inodo_dir=p_entrada; // se refiere a esto?
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
    return EXITO;
             }



void mostrar_error_buscar_entrada(int error) {
    // fprintf(stderr, "Error: %d\n", error);
    switch (error) {
        case -2: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
        case -3: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
        case -4: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
        case -5: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
        case -6: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
        case -7: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
        case -8: fprintf(stderr, "Error: No es un directorio.\n"); break;
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