#include "tarfunc.h"
#include "encrypt.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>

/*
* mytar - Empaquetador de archivos de un directorio
* --------------------
* mytar es una libreria para empaquetar archivos dentro
* de un directorio o un archivo solo. 
* Ofrece funciones como: empaquetar, extraer, mostrar contenido, y cifrado
* Para ejecutar alguna de sus funciones vease los comandos:
* -f,-c,-n,-t,-o,-z,-y,-x,-s,-v
*
* Para obtener una descripcion detallada de cada comando, vease mytar man 
*
* Autores:    Antonella Requena 15-11196
*             Ezequiel Zabala 14-11160
* Ultima fecha de modificacion: 31/09/2019
*/

/*
* Funcion: Match (expresion regular)
* --------------------
* Utiliza la libreria regex para que, dado un patron y un string
* determine si el string matchea al patron dado.
* compila la expresion, y luego libera el espacio creado
* retorna 1 si matchea y 0 si no
*
* string: palabra a comparar con regex
* pattern: patron para construir la expresion regular
*/
int match(char *string, const char *pattern)
{
    regex_t re;
    int status;
    if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB) != 0)
        return 0;
    status = regexec(&re, string, 0, NULL, 0);
    regfree(&re);
    if (status != 0)
        return 0;
    return 1;
}

/*
* Funcion: Manejo de entrada
* --------------------
* Considera casos especiales de entrada que getopt no puede manejar
* Caso especial 1: que el usuario ingrese "man" para ver el manual
* Otros casos: errores en el formato de comandos
* Imprime por pantalla el error y retorna 1 para terminar el programa
*
* input: string a considerar de la entrada, se asume que el error esta en argv[1]
*/
int iohandler(char *input)
{
    /* Si el usuario quiere ver el manual */
    if (match(input, "man"))
    {
        FILE *manual;
        manual = fopen("man", "r");
        char ch;
        while ((ch = fgetc(manual)) != EOF)
            printf("%c", ch);
        fclose(manual);
        return 1;
    }
    /* Si ingresa alguna combinacion de comandos invalidas */
    if ((strlen(input) > 2) && (match(input, "-[cv]+f|-[xt]f") == 0))
    {
        printf("Comando incorrecto. La opción f debe ser la más a la derecha\n");
        return 1;
    }

    /* Si ingresa un formato invalido de comando (mas de 3 caracteres) */
    if (strlen(input) > 4)
    {
        printf("Formato de comando incorrecto.\n");
        return 1;
    }
}

/*
* Funcion: Main
* --------------------
* Se maneja la lectura por comando las funciones del tar son triggereadas a partir
* de los flags indicados, para esto se hace uso de la libreria getopt y una estructura de seleccion 
* multiple (switch case) para manejar cada caso
*
* argc: numero de archivos en la linea de comandos
* argv: arreglo de opciones (flags), y argumentos
*/
int main(int argc, char *argv[])
{
    /* Variables para manejar opciones */

    /* VALUES */
    char *fvalue;
    char *ovalue;
    char *vvalue;
    char *svalue;
    int zvalue;
    int yvalue;

    /* FLAGS */
    int cflag;
    int nflag;
    int tflag;
    int xflag;
    int vflag;

    /* Variables de la libreria getopt */
    int optopt;
    int opterr;
    char *optarg;
    int optind;

    int index;         /* Variable utilizada para recorrer arreglo de argumentos extra */
    int opt;           /* Variable utilizada para evaluar los casos que retorna getopt */
    char *extrargs[3]; /* Arreglo de caracteres para guardar argumentos considerados adiconales por la libreria */

    /* Seteamos esta variable en 0 para que la libreria getopt no imprima
    sus mensajes de error por defecto*/
    opterr = 0;

    /* Verificamos posibles errores de IO o si el usuario desea ver manual */
    iohandler(argv[1]);

    /* Manejador de opciones de comando */
    while ((opt = getopt(argc, argv, "f:cnto:z:y:xs:v")) != -1)

        switch (opt)
        {
        case 'f': /* especifica el tar a usar*/
            fvalue = optarg;
            break;
        case 'c': /* permite crear el tar */
            cflag = 1;
            break;
        case 'n': /* ignora archivos que no sean regulares o directorios */
            nflag = 1;
            break;
        case 't': /* muestra el contenido del tar */
            tflag = 1;
            break;
        case 'o': /* salida de una extracción */
            ovalue = optarg;
            break;
        case 'z': /* cifrar archivo mytar */
            zvalue = atoi(optarg);
            break;
        case 'y': /* descifrar archivo mytar */
            yvalue = atoi(optarg);
            break;
        case 'x': /* extraer todo */
            xflag = 1;
            break;
        case 's': /* extrae un archivo especifico del mytar */
            svalue = optarg;
            break;
        case 'v': /* verbosity */
            vvalue = optarg;
            break;

        /* Caso en el que reporta un comportamiento no esperado */
        case '?':
            /* Si opt coincide con algun caracter opcion con argumento, 
               es porque alguno de ellos fue ingresado sin su respectivo argumento */
            if (optopt == 'f' || optopt == 'o' || optopt == 'z' || optopt == 'y' || optopt == 's')
                fprintf(stderr, "La opción -%c necesita un argumento.\n", optopt);

            /* Si optopt posee otra cosa que es imprimible, es porque se leyo un caracter
               no admisible en las opciones */
            else if (isprint(optopt))
                fprintf(stderr, "Opción desconocida '-%c'\n", optopt);

            return 1;
        /* Default case, aborta el programa */
        default:
            abort();
        }

    /* Registramos los argumentos adicionales (path de directorios) en un arreglo */
    int i = 0;
    for (index = optind; index < argc; index++)
    {
        extrargs[i] = argv[index];
        i++;
    }

    /* AHORA HACEMOS LAS LLAMADAS CORRESPONDIENTES A CADA CASO */

    /* Opcion -cnf: crear tar, -z especificado */
    if (fvalue != NULL && cflag != 0)
        tar_create(fvalue, extrargs[0], nflag, zvalue);

    /* Opcion -tf: Mostrar contenido del tar en CLI */
    if (fvalue != NULL && tflag != 0)
        tar_print(fvalue, 0, yvalue);

    /* Opcion -xf: Extraer archivos */
    if (fvalue != NULL && xflag != 0)
    {
        tar_extract(fvalue, yvalue, ovalue, NULL);
    }
    /* Opcion -xs: Extraer un archivo */
    if (fvalue != NULL && svalue != NULL)
    {
        tar_extract(fvalue, yvalue, ovalue, svalue);
    }

    return 0;
}