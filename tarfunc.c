#include "tarfunc.h"
#include "listdir.h"
#include "encrypt.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

#define V_PRINT(f, fmt, ...)                 \
    if (verbosity)                           \
    {                                        \
        fprintf(f, fmt "\n", ##__VA_ARGS__); \
    }

struct tar_header
{
    char name[100];
    char mode[9];
    char type[1];
    char owner[20];
    char group[20];
    long size;
};

/*
* Funcion: Agregar entrada al tar
* --------------------
* Recibe como parametro el archivo de extension .mytar
* donde va a colocar los archivos empaquetados, el archivo
* que se desea colocar en el .mytar y un entero para saber si ignora
* archivos que no sean regulares o directorio.
*
* Recorre el directorio recursivamente para agregar todos los archivos
* que esten dentro de el al tar
*
* 
* tar: FILE donde se va a escribir el empaquetado
* filepath: char con el nombre del archivo a empaquetar
* ignoreFiles: 1 si hay que ignorarlos, 0 sino
*/
void tar_add(FILE *tarfile, const char *filename)
{
    /* Get current position; round to a multiple of 512 if we aren't there already */
    size_t index = ftell(tarfile);
    size_t offset = index % 512;
    if (offset != 0)
    {
        fexpand(tarfile, 512 - offset, 0);
    }
    //Store the index for the header to return to later
    index = ftell(tarfile);
    //Write some space for our header
    fexpand(tarfile, sizeof(struct tar_header), 0);
    //Write the input file to the tar file
    FILE *input = fopen(filename, "rb");
    if (input == NULL)
    {
        fprintf(stderr, "Failed to open %s for reading\n", filename);
        return;
    }
    //Copy the file content to the tar file
    while (!feof(input))
    {
        char buffer[2000];
        size_t read = fread(buffer, 1, 2000, input);
        fwrite(buffer, 1, read, tarfile);
    }
    //Get the end to calculate the size of the file
    size_t end = ftell(tarfile);
    //Round the file size to a multiple of 512 bytes
    offset = end % 512;
    if (end != 0)
    {
        fexpand(tarfile, 512 - offset, 0);
    }
    //Fill out a new tar header
    struct tar_header header;
    memset(&header, 0, sizeof(struct tar_header));
    snprintf(header.name, 100, "%s", filename);
    snprintf(header.mode, 20, "%06o ", 0777); //You should probably query the input file for this info
    snprintf(header.owner, 20, "%06o ", 0);   //^
    snprintf(header.group, 20, "%06o ", 0);   //^
    snprintf(header.size, 12, "%011o", end - 512 - index);
    header.type[0] = '0';

    //Save the new end to return to after writing the header
    end = ftell(tarfile);

    //Write the header
    fseek(tarfile, index, SEEK_SET);
    //  fwrite( bytes, 1, sizeof( struct tar_header ), tarfile );

    //Return to the end
    fseek(tarfile, end, SEEK_SET);
    fclose(input);
}

/*
* Funcion: Recorrer Directorio
* --------------------
* Recibe como parametro el archivo de extension .mytar
* donde va a colocar los archivos empaquetados, el archivo
* que se desea colocar en el .mytar y un entero para saber si ignora
* archivos que no sean regulares o directorio.
*
* Recorre el directorio recursivamente para agregar todos los archivos
* que esten dentro de el al tar
*
* 
* tar: FILE donde se va a escribir el empaquetado
* filepath: char con el nombre del archivo a empaquetar
* ignoreFiles: 1 si hay que ignorarlos, 0 sino
*/
int walk_dir(FILE *tar, const char *filepath, int ignoreFiles)
{
    DIR *dir;
    struct dirent *entry;
    char buffer[2000];
    size_t read;

    if (!(dir = opendir(filepath)))
        return 1;

    while ((entry = readdir(dir)) != NULL)
    {
        /* Si el archivo es de tipo directorio */
        if (entry->d_type == 4)
        {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", filepath, entry->d_name);
            walk_dir(tar, path, ignoreFiles);
        }
        /* Si el archivo es de otro tipo que no es regular o directorio */
        /* y el usuario pidio por linea de comandos que se ignorasen*/
        else if (ignoreFiles == 1 && entry->d_type != 5)
            continue;

        /* Si el archivo coincide con el suministrado para la salida 
        /* se ignora */
        else if (strcmp(entry->d_name, tar) == 0)
            continue;

        /* Sino, se agrega al archivo .mytar */
        else
            tar_add(tar, entry->d_name);
    }
    /* Al finalizar, cerramos el directorio */
    closedir(dir);
}

/*
* Funcion: Crear archivo mytar
* --------------------
* Recibe como parametro el archivo de extension .mytar
* donde va a colocar los archivos empaquetados, y el archivo
* que se desea colocar en el .mytar.
* Notese que el archivo puede ser de cualquier tipo.
* si el archivo .mytar ya poseia algo, lo sobreescribe
* 
* LO QUE HACE
*
* string: palabra a comparar con regex
* pattern: patron para construir la expresion regular
*/
int tar_create(char *tarname, char *filename, int ignoreFiles, int shiftBytes)
{

    size_t index; /* Variable para */

    /*  Creamos un archivo de nombre tarfile */
    FILE *tarfile = fopen(tarname, "wr");

    /* Abrimos el archivo a empaquetar para lectura/binario */
    FILE *inputfile = fopen(filename, "rb");

    index = ftell(tarfile);

    /* Si el apuntador es NULL*/
    if (inputfile == NULL)
    {
        fprintf(stderr, "No se puede abrir el archivo %s para lectura\n", filename);
        return 1;
    }

    /* Creamos el espacio para la metadata */
    struct tar_header headers[20000];

    /*  Lo guardamos al inicio del archivo */
    fwrite(headers, 6, sizeof(headers), tarfile);

    /* Ahora agregamos nuevas entradas al archivo mytar */
    walk_dir(tarfile, filename, ignoreFiles);

    /* Finalmente, ciframos el archivo shiftBytes bytes */

    if (shiftBytes != 0)
        encryptArchive(tarfile, shiftBytes);
}