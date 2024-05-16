# Adelaida
# scripte2.sh

clear
rm -rf disco* ext* res*
make clean
make
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkfs disco 100000\x1b[0m"
./mi_mkfs disco 100000
echo
echo -e "\x1B[38;2;17;245;120m$ ./leer_sf disco\x1b[0m"
./leer_sf disco
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120mNIVEL 8: CREAR DIRECTORIOS Y FICHEROS, PERMISOS, STAT, LISTAR\x1b[0m"
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir #comprobar sintaxis\x1b[0m"
./mi_mkdir 
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 7 / #no ha de dejar crear la raíz al usuario\x1b[0m"
./mi_mkdir disco 7 /
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 dir1/\x1b[0m"
./mi_mkdir disco 6 dir1/
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir1/\x1b[0m"  
./mi_mkdir disco 6 /dir1/   
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir1/dir11/\x1b[0m"
./mi_mkdir disco 6 /dir1/dir11/  
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_chmod #comprobar sintaxis\x1b[0m"
./mi_chmod 
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_chmod disco 1 /dir1/dir11/ #cambio a permiso ejecución\x1b[0m"
./mi_chmod disco 1 /dir1/dir11/  
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic111 #Error: Permiso denegado de lectura.\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic111  
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_chmod disco 2 /dir1/dir11/ #cambio a permiso escritura\x1b[0m"
./mi_chmod disco 2 /dir1/dir11/  
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic111 #Error: Permiso denegado de lectura.\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic111  
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_chmod disco 6 /dir1/dir11/ #cambio a permiso lectura/escritura\x1b[0m"
./mi_chmod disco 6 /dir1/dir11/   
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic111\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic111 
echo 
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic112\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic112  
echo
echo -e "\x1B[38;2;17;245;120m"
echo "                  /"
echo "                  |"
echo "                dir1"
echo "                  |"
echo "                dir11"
echo "               /     \\"
echo "           fic111   fic112"
echo -e "\x1b[0m"
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_ls disco /\x1b[0m"
./mi_ls disco /
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/\x1b[0m"
./mi_stat disco /dir1/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_ls disco /dir1/\x1b[0m"
./mi_ls -l disco /dir1/ 
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/dir11/\x1b[0m"
./mi_stat disco /dir1/dir11/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_ls disco /dir1/dir11/\x1b[0m"
./mi_ls -l disco /dir1/dir11/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_ls disco /dir1/dir12/ #Error: No existe el archivo o el directorio.\x1b[0m"
./mi_ls -l disco /dir1/dir12/
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic111 #Error: El archivo ya existe.\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic111  
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir1/dir11/fic111/dir12/ #Error: No es un directorio.\x1b[0m"
./mi_mkdir disco 6 /dir1/dir11/fic111/dir12/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/dir12/fic111 #Error: No existe algún directorio intermedio.\x1b[0m"
./mi_touch disco 6 /dir1/dir11/dir12/fic111
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 9 /dir2/ #Error: modo inválido: <<9>>\x1b[0m"
./mi_mkdir disco 9 /dir2/
echo
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120mNIVEL 9: LEER Y ESCRIBIR\x1b[0m"
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./leer_sf disco\x1b[0m"
./leer_sf disco
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir  disco /dir1/dir11/fic111 \"$(cat texto2.txt)\" 0 #⊂ BL 0 ∈ D0\x1b[0m"
echo -e "\x1B[38;2;17;245;120m#reservaremos 5 bloques de datos (3 escritos completos y 2 parciales)\x1b[0m"
./mi_escribir  disco /dir1/dir11/fic111 "$(cat texto2.txt)" 0
echo
echo -e "\x1B[38;2;17;245;120m$ ./leer_sf disco\x1b[0m"
./leer_sf disco
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_cat  disco /dir1/dir11/fic111>ext1.txt\x1b[0m"
./mi_cat  disco /dir1/dir11/fic111>ext1.txt
echo
echo -e "\x1B[38;2;17;245;120m$ ls -l ext1.txt #comprobamos el tamaño del fichero externo\x1b[0m" 
ls -l ext1.txt
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_chmod disco 4 /dir1/dir11/fic111 #cambio a permiso lectura\x1b[0m"
./mi_chmod disco 4 /dir1/dir11/fic111
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir  disco /dir1/dir11/fic111 \"lo que sea\" 209000 #⊂ BL 204 ∈ I0\x1b[0m"
./mi_escribir  disco /dir1/dir11/fic111 "lo que sea" 209000
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_ls disco /dir1/dir11/\x1b[0m"
./mi_ls -l disco /dir1/dir11/
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir disco /dir1/dir11/fic112 \"hola1\" 209000 #⊂ BL 204 ∈ I0\x1b[0m"
echo -e "\x1B[38;2;17;245;120m#modifica tamEnBytesLog, mtime y ctime, y reservamos 1 bloque datos y 1 de punteros\x1b[0m"
./mi_escribir disco /dir1/dir11/fic112 "hola1"  209000
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/dir11/fic112\x1b[0m"
./mi_stat disco /dir1/dir11/fic112
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ sleep 1 #esperamos para distanciar los sellos de tiempo\x1b[0m"
sleep 1
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir disco /dir1/dir11/fic112 \"hola2\" 9000 #⊂ BL 8 ∈ D8\x1b[0m"
echo -e "\x1B[38;2;17;245;120m#no cambia tamenBytesLog pero sí mtime y ctime (ocupamos 1 bloque más de datos)\x1b[0m"
./mi_escribir disco /dir1/dir11/fic112 "hola2" 9000 
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/dir11/fic112\x1b[0m"
./mi_stat disco /dir1/dir11/fic112
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ sleep 1 #esperamos para distanciar los sellos de tiempo\x1b[0m"
sleep 1 
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir disco /dir1/dir11/fic112 \"hola3\" 9100 #⊂ BL 8 ∈ D8\x1b[0m"
echo -e "\x1B[38;2;17;245;120m#mismo bloque que offset 9000, cambia mtime pero no ctime\x1b[0m"
./mi_escribir disco /dir1/dir11/fic112 "hola3"  9100
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/dir11/fic112\x1b[0m"
./mi_stat disco /dir1/dir11/fic112
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ sleep 1 #esperamos para distanciar los sellos de tiempo\x1b[0m"
sleep 1
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir disco /dir1/dir11/fic112 \"hola4\" 275000 #⊂ BL 268 ∈ I1\x1b[0m" 
echo -e "\x1B[38;2;17;245;120m#cambia tamEnBytesLog, mtime y ctime, y reservamos 1 bloque datos y 2 de punteros\x1b[0m"
./mi_escribir disco /dir1/dir11/fic112 "hola4" 275000
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat /dir1/dir11/fic112 \x1b[0m"
./mi_stat disco /dir1/dir11/fic112 
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_cat /dir1/dir11/fic112 \x1b[0m"
./mi_cat disco /dir1/dir11/fic112 
echo
echo -e "\x1B[38;2;17;245;120m######################################################################\x1b[0m"
make clean