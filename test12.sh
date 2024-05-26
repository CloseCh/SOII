rm -rf disco* ext* res*
make clean
make
clear
echo -e "\x1B[38;2;17;245;120m$ ./mi_mkfs disco 100000\x1b[0m"
./mi_mkfs disco 100000
echo
echo -e "\x1B[38;2;17;245;120m$ Realizando simulacion\x1b[0m"
time ./simulacion disco
echo
echo -e "\x1B[38;2;17;245;120m$ Realizando verificacion\x1b[0m"
time ./verificacion disco