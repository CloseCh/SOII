Se ha realizado un cambio de sintaxis sobre leer_sf.c. Para ejecutar las pruebas se ha de introducir el siguiente sintaxis:

	./leer_sf <nombre disco> <nivel a probar>

Se ha realizado este cambio de sintaxis para cuando hay fallos en niveles anteriores poder realizar más fácil las pruebas.

A este cambio se añade un nuevo programa llamado pruebas_fichero.c (con su correspondiente .h), el cual se compila directamente junto a los otros programas (añadido al makefile).

Sobre los .sh se ha realizado el cambio de sintaxis de leer_sf. 