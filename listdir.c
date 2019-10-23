#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "listdir.h"

#define DT_DIR DT_DIR

/*
* Listar directorio
* --------------------
* Indica los archivos y subdirectorios de un directorio
* Autores: Antonella Requena 15-11196
*          Ezequiel Zabala 14-11160
*
* Ultima fecha de modificacion: 22/09/2019
*/



/*
* Funcion: Listar directorio
* --------------------
* Revisa si ya existe la palabra en la lista, si existe aumenta el contador y retorna
* sino, crea el nodo y lo guarda
* Usamos una variable auxiliar newnode como puntero al nodo que se esta revisando
* 
* head: puntero de la cabeza de la lista
* newword: puntero de string a insertar
* return: el puntero de la cabeza
*/

void listdir(const char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type != NULL)
        {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            listdir(path, indent + 2);
        }
        else
        {
            printf("%*s- %s\n", indent, "", entry->d_name);
        }
    }
    closedir(dir);
}

