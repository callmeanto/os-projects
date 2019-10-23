#include "listdir.h"

/*
* tar - Empaquetador de archivos de un directorio
* --------------------
* Se leen las palabras de los archivos, se insertan en una lista
* Si la palabra ya existia en la lista, se suma 1 al contador de frecuencia
* de la palabra, sino, se agrega como un nodo nuevo. Luego de leer todos
* los archivos, se imprime la lista ordenada y finalmente se libera el espacio
* 
* Autores:    Antonella Requena 15-11196
*             Ezequiel Zabala 14-11160
* Ultima fecha de modificacion: 22/09/2019
*/

/*
* Funcion: Main
* --------------------
* Se maneja la lectura de archivos y se utilizan las funciones de la libreria 
* 'list' para llenar la lista, imprimir y liberar el espacio al terminar
*
* argc: numero de archivos en la linea de comandos
* argv: arreglo de ruta de los archivos a leer
*/
int main(void)
{
    listdir(".", 0);
    return 0;
}