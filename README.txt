Se ha realizado un cambio de sintaxis sobre leer_sf.c. Para ejecutar las pruebas se ha de introducir el siguiente sintaxis:

	./leer_sf <nombre disco> <nivel a probar>

Se ha realizado este cambio de sintaxis para cuando hay fallos en niveles anteriores poder realizar más fácil las pruebas.

A este cambio se añade un nuevo programa llamado pruebas_fichero.c (con su correspondiente .h), el cual se compila directamente junto a los otros programas (añadido al makefile).

Sobre los .sh se ha realizado el cambio de sintaxis de leer_sf. 

Mejoras realizadas: 
- Lectura y escritura en liberar_bloques_inodo:
	Se ha utilizado la versión iterativa aplicando las dos mejoras de lectura y la mejora de escritura.

- Buffer de entradas en buscar_entrada() y mi_dir()
	Se realiza la lectura de un bloque de entradas para posteriormente iterar sobre ella.

- Visualización de mi_ls
	La función mi_dir() diferencia entre ficheros y flag.
	Se accepta:
		- Normal, sin flag, imprime los directorios y ficheros con el mismo color
		- Con flag -l, para mostrar datos sobre cada entrada
		- Con diferenciación entre fichero y directorio
			Esta diferenciación es realizado desde el mi_ls realizando la llamada de mi_dir() tras pasar unas comparaciones.

- mi_touch y mi_rmdir
	Es realizado por comparaciones en las propias funciones para diferenciar entre fichero y directorio para poder llevar a cabo todo.

