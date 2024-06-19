#include "bloques.h"
#include "semaforo_mutex_posix.h"

static unsigned int descriptor = 0;

static sem_t *mutex;

static unsigned int inside_sc = 0;

#if MMAPOPEN
    static int tamSFM;

    static void *ptrSFM;
#endif

#if MMAPOPEN
    int bmount(const char *camino){
        //No aparecen permisos desenmascarar
        umask(000);

        //Para los procesos hijo
        if (descriptor > 0){
            close(descriptor);
        }

        //open(camino,oflags,mode)
        descriptor = open(camino, O_RDWR | O_CREAT, 0666);

        if (descriptor == -1) {
            fprintf(stderr, RED"Error: creacion de disco\n"RESET);
            return FALLO;
        }

        ptrSFM = do_mmap(descriptor);

        //Inicializar semaforo
        if (!mutex) { // el semáforo es único en el sistema y sólo se ha de inicializar 1 vez (padre)
            mutex = initSem(); //lo inicializa a 1
            if (mutex == SEM_FAILED) {
                return -1;
            }
        }

        return descriptor;
    }
#else
    int bmount(const char *camino){
        //No aparecen permisos desenmascarar
        umask(000);

        //Para los procesos hijo
        if (descriptor > 0){
            close(descriptor);
        }

        //open(camino,oflags,mode)
        descriptor = open(camino, O_RDWR | O_CREAT, 0666);
        if (descriptor == -1) {
            fprintf(stderr, RED"Error: creacion de disco\n"RESET);
            return FALLO;
        }

        //Inicializar semaforo
        if (!mutex) { // el semáforo es único en el sistema y sólo se ha de inicializar 1 vez (padre)
            mutex = initSem(); //lo inicializa a 1
            if (mutex == SEM_FAILED) {
                return -1;
            }
        }

        return descriptor;
    }
#endif

#if MMAPOPEN
    int bumount(){
        //Volcar el contenido actualizado o no
        write(descriptor, ptrSFM, BLOCKSIZE);

        descriptor = close(descriptor); //close() devuelve 0 en caso de éxito
        
        if (descriptor == -1){
            fprintf(stderr, RED"Error al desmontar dispositivo\n"RESET);
        }
        
        //Eliminar semaforo
        deleteSem(); 

        return descriptor;
    }
#else
    int bumount(){
        descriptor = close(descriptor); //close() devuelve 0 en caso de éxito
        unmap();
        
        if (descriptor == -1){
            fprintf(stderr, RED"Error al desmontar dispositivo\n"RESET);
        }
        
        //Eliminar semaforo
        deleteSem(); 

        return descriptor;
    }
#endif

#if MMAPOPEN
    int bwrite(unsigned int nbloque, const void *buf){
        unsigned int s;

        if (nbloque*BLOCKSIZE + BLOCKSIZE <= tamSFM){
            s = BLOCKSIZE;
        } else {
            s = tamSFM - nbloque*BLOCKSIZE;
        }

        if (s > 0) memcpy(ptrSFM + nbloque*BLOCKSIZE, buf, s);
        
        return s;
    }
#else
    int bwrite(unsigned int nbloque, const void *buf){
        //calculamos desplazamiento
        unsigned int desplazamiento = nbloque * BLOCKSIZE;

        //movemos el puntero del fichero en el offset correcto
        lseek(descriptor, desplazamiento, SEEK_SET);

        //volcamos el contenido del buffer en la posicion del dv
        unsigned int size = write(descriptor, buf, BLOCKSIZE);

        /*En cada funcion posterior se escribirá por terminal 
        de donde proviene*/
        if (size != BLOCKSIZE) {
            fprintf(stderr, RED"Error: escribir bloque\n"RESET);
            return FALLO;
        }
        
        return size;
    }
#endif

#if MMAPOPEN
    int bread(unsigned int nbloque, void *buf){
        unsigned int s;

        if (nbloque*BLOCKSIZE + BLOCKSIZE <= tamSFM){
            s = BLOCKSIZE;
        } else {
            s = tamSFM - nbloque*BLOCKSIZE;
        }

        if (s > 0) memcpy(buf, ptrSFM + nbloque*BLOCKSIZE, s);
        
        return s;
    }
#else
    int bread(unsigned int nbloque, void *buf){
        //calculamos desplazamiento
        unsigned int desplazamiento = nbloque * BLOCKSIZE;

        // movemos el puntero del fichero en el offset correcto
        lseek(descriptor, desplazamiento, SEEK_SET);

        unsigned int size = read(descriptor, buf, BLOCKSIZE);

        /*En cada funcion posterior se escribirá por terminal 
        de donde proviene*/
        if (size != BLOCKSIZE) {
            fprintf(stderr, RED"Error: leer bloque\n"RESET);
            return FALLO;
        }
        
        return size;
    }
#endif

void mi_waitSem() {
    if (!inside_sc) { // inside_sc==0, no se ha hecho ya un wait
        waitSem(mutex);
    }
    inside_sc++;
}


void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}

#if MMAPOPEN
    void *do_mmap(int fd) {
        struct stat st;
        void *ptr;
        fstat(fd, &st);
        tamSFM = st.st_size; //static int tamSFM: tamaño memoria compartida
        if ((ptr = mmap(NULL, tamSFM, PROT_WRITE, MAP_SHARED, fd, 0))== (void *)-1)
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno)); 
        return ptr;
    }
#endif