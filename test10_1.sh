# Adelaida
# test10.sh
clear
make clean
make

echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m                          1ª parte\x1b[0m"
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkfs disco 100000\x1b[0m"
./mi_mkfs disco 100000
echo
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir1/\x1b[0m"
./mi_mkdir disco 6 /dir1/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir1/dir11/\x1b[0m"
./mi_mkdir disco 6 /dir1/dir11/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic1\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic1
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_escribir disco /dir1/dir11/fic1 hellooooooo 0\x1b[0m"
./mi_escribir disco /dir1/dir11/fic1 "hellooooooo" 0
echo
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir2/\x1b[0m"
./mi_mkdir disco 6 /dir2/
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkdir disco 6 /dir2/dir21/\x1b[0m"
./mi_mkdir disco 6 /dir2/dir21/
echo
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2\x1b[0m"
./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2
echo
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_cat disco /dir2/dir21/fic2 #ha de mostrar mismo contenido que /dir1/dir11/fic1\x1b[0m"
./mi_cat disco /dir2/dir21/fic2 
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/dir11/fic1\x1b[0m"
./mi_stat disco /dir1/dir11/fic1 
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir2/dir21/fic2 #han de tener el mismo inodo y 2 enlaces\x1b[0m"
./mi_stat disco /dir2/dir21/fic2
echo
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_link disco /dir1/dir11/fic3 /di2/dir21/fic4  #camino1 ha de existir\x1b[0m"
./mi_link disco /dir1/dir11/fic3 /di2/dir21/fic4
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_touch disco 6 /dir1/dir11/fic3\x1b[0m"
./mi_touch disco 6 /dir1/dir11/fic3 
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4\x1b[0m"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5\x1b[0m"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5
echo
echo -e "\x1B[38;2;17;245;120m$ ./mi_stat disco /dir1/dir11/fic3\x1b[0m"
./mi_stat disco /dir1/dir11/fic3
echo
echo -e "\x1B[38;2;17;245;120m##############################################################\x1b[0m"
echo -e "\x1B[38;2;17;245;120m$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2 #camino2 NO ha de existir\x1b[0m"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2 #camino2 NO ha de existir 