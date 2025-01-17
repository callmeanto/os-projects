#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listdir.h"

#define putc(_ch,_fp) _IO_putc (_ch, _fp)

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
* Recorre el directorio dado en la entrada recursivamente, 
* si el archivo que esta revisando es directorio, imprime
*
* name: puntero del nombre del archivo (dir o regular)
* indent: espacio de indentacion con el que imprime
*
*/

void listdir(const char *name, int indent)
{
    DIR *dir;
    FILE *fp1,*fp2;
    char c;
    struct dirent *entry;
    char buffer[2000];
    size_t read;

    fp2 = fopen("output.txt", "w");

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 4)
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
            fwrite(entry->d_name, 1, sizeof(entry->d_name), fp2);
        }
 
    }
    closedir(dir);

       c = fgetc(fp2); 
        while (c != EOF) 
        { 
            printf ("%c", c); 
            c = fgetc(fp2); 
        }
}

