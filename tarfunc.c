#include "tarfunc.h"
#include "encrypt.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

struct tar_header
{
    char name[100];
    char mode[9];
    char type[1];
    char owner[20];
    char group[20];
    long size;
} header;

/*
* Funcion: Verbosidad
* --------------------
* Se invoca sta función cada vez que se quiera imprimir en pantalla o en un
* archivo lo que el programa está haciendo. Si se le pasa un apuntador FILE
* vacío imprime en pantalla. Si el puntero apunta a una archivo abierto
* imprime lo que se encuentra en "line" en el archivo referenciado por 
* por foutput.
*
* line: linea que se quiere imprimir en pantalla o almacenar en un archivo
* foutput: apuntador a un archivo de registro
*/
void verbose(char *line, FILE *foutput)
{
    if (foutput != NULL)
    {
        fprintf(foutput,line);
        fprintf(foutput,"\n");
    }
    else
        printf("%s",line);
}

/**
 * Funcion: expandir espacio para archivo
 * --------------------
 * Recibe un archivo, la cantidad de espacio a "expandir" y el
 * valor a colocar en el archivo
 * 
 * f: apuntador al archivo
 * amount: cantidad de espacio
 * value: valor a colocar
 */
void fexpand(FILE* f, size_t amount, int value){
    while( amount-- ){
        fputc( value, f );
    }
}

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
void tar_add(char *tarname, const char *filename, int v, FILE *foutput)
{
    if(v)verbose("Vamos a agregar un archivo al mytar. ",foutput);

    if(v)verbose("Abrimos el archivo.\n",foutput);
    /*  Creamos un archivo de nombre tarfile */
    FILE *tarfile = fopen(tarname, "wr");

    /* Obtener indice de posicion actual en el archivo */
    size_t index = ftell(tarfile);
    size_t offset = index % 512;
    if (offset != 0)
    {
        fexpand(tarfile, 512 - offset, 0);
    }

    /* Guardamos el indice */
    index = ftell(tarfile);

    /* Creamos espacio para el header */
    fexpand(tarfile, sizeof(struct tar_header), 0);

    /* Write the input file to the tar file */ 
    FILE *input = fopen(filename, "rb");
    if (input == NULL)
    {
        if(v)verbose("No se pudo abrir el archivo.\n",foutput);
        fprintf(stderr, "Failed to open %s for reading\n", filename);
        return;
    }
    //Copy the file content to the tar file
    while (!feof(input))
    {
        if(v)verbose("Copiamos su contenido a archivo mytar.\n",foutput);
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
    if(v)verbose("Llenamos el header del archivo a empaquetar\n",foutput);
    struct tar_header header;
    memset(&header, 0, sizeof(struct tar_header));
    snprintf(header.name, 100, "%s", filename);
    snprintf(header.mode, 20, "%06o ", 0777);
    snprintf(header.owner, 20, "%06o ", 0);   
    snprintf(header.group, 20, "%06o ", 0);   
    snprintf(header.size, 12, "%011o", end - 512 - index);
    header.type[0] = '0';

    //Save the new end to return to after writing the header
    end = ftell(tarfile);

    //Write the header
    if(v)verbose("Agregamos el header al mytar.\n",foutput);
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
int walk_dir(char *tar, const char *filepath, int ignoreFiles, int v, FILE *foutput)
{
    DIR *dir;
    struct dirent *entry;
    size_t read;

    if(v)verbose("Recorremos un directorio para agregar su contenido al archivo mytar.\n",foutput);
    if (!(dir = opendir(filepath)))
        return 1;

    while ((entry = readdir(dir)) != NULL)
    {
        /* Si el archivo es de tipo directorio */
        if (entry->d_type == 4)
        {
            if(v)verbose("Entro a un subdirectorio.\n",foutput);
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", filepath, entry->d_name);
            if(v)verbose("Lo recorro agregando su contenido.\n",foutput);
            walk_dir(tar, path, ignoreFiles, v, foutput);
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
            if(v)verbose("Encuentro un archivo valido, lo agrego a mytar.\n",foutput);
            tar_add(tar, entry->d_name, v, foutput);
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
* string: palabra a comparar con regex
* pattern: patron para construir la expresion regular
*/
int tar_create(char *tarname, char *filename, int ignoreFiles, int shiftBytes, int v, FILE *foutput)
{

    size_t index; /* Variable con apuntador al ultimo caracter leido del archivo */

    if(v)verbose("Creamos el archivo tar.\n",foutput);
    /*  Creamos un archivo de nombre tarfile */
    FILE *tarfile = fopen(tarname, "wr");

    if(v)verbose("Abrimos el archivo a empaquetar.\n",foutput);
    /* Abrimos el archivo a empaquetar para lectura/binario */
    FILE *inputfile = fopen(filename, "rb");

    index = ftell(tarfile);

    /* Si el apuntador es NULL*/
    if (inputfile == NULL)
    {   
        if(v)verbose("El archivo que se desea empaquetar no se pudo abrir.\n",foutput);
        fprintf(stderr, "No se puede abrir el archivo %s para lectura\n", filename);
        return 1;
    }

    /* Creamos el espacio para la metadata */
    struct tar_header headers[2000];

    if(v)verbose("Se crea el espacio para su metadata y se guarda en mytar.\n",foutput);
    /*  Lo guardamos al inicio del archivo */
    fwrite(headers, sizeof(header), sizeof(headers), tarfile);

    if(v)verbose("Vamos a agregar otro archivo en el archivo mytar.\n",foutput);
    /* Ahora agregamos nuevas entradas al archivo mytar */
    walk_dir(tarname, filename, ignoreFiles);

    /* Finalmente, ciframos el archivo shiftBytes bytes */
    if (shiftBytes != 0){
        if(v)verbose("Realizamos el cifrado del archivo mytar final.\n",foutput);
        encryptArchive(tarfile, shiftBytes, v, foutput);
    }
}

/*
* Funcion: Extraer archivo mytar
* --------------------
* Recorre todo el .mytar revisando los headers de cada
* archivo, revisando su path name, creando un directorio
* por cada / en el nombre del archivo, si el directorio ya existia
* no lo crea, y coloca recursivamente los archivos que van dentro de cada /
* cuando llega a una hoja, crea un archivo con fopen y le asigna los atributos
* especificados en su header 
*
* tarname: nombre del archivo
* yvalue: llave para desencriptar archivo
* ovalue: nombre del directorio donde colocar los archivos a extraer
* extractFile: si es distinto de null, la funcion debe extraer solo este archivo
*/
int tar_extract(char *tarname, int yvalue, char *ovalue, char *extractFile, int v, FILE *foutput){
    DIR* outputDir; 

    if(v)verbose("Vamos a extraer los archivos del mytar ",foutput);
    /* Si el usuario no especifico argumento para el flag -o, se toma el directorio
    * actual por defecto  */
    if(ovalue==NULL){
        if(v)verbose("en el directorio actual.\n",foutput);
        outputDir = mkdir(".","rw");
    }
    else
        if(v)verbose("en el directorio especificado.\n",foutput);
        outputDir = mkdir(ovalue,"rw");
    
        
}


/*
* Funcion: Imprimir contenido del tar en CLI
* --------------------
* Recorre todo el .mytar revisando los headers de cada
* archivo, imprimiendo el contenido de cada header (nombre, permisos, owner, grupo), 
* avanzando header + size para acceder a cada header 
*
* tarname: nombre del archivo tar a imprimir
* indent: cantidad de espacios de indentacion (formato de impresion)
*/
int tar_print(char *tarname, int indent, int v, FILE *foutput){
    DIR *dir;
    FILE *fp1,*fp2;
    struct dirent *entry;
    size_t read;

    if(v)verbose("Vamos a recorrer el archivo mytar imprimiendo su contenido.\n",foutput);

    fp2 = fopen("output.txt", "w");

    if (!(dir = opendir(tarname)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 4)
        {   
            if(v)verbose("Entro a un subdirectorio.\n",foutput);
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", tarname, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);           
            listdir(path, indent + 2);
        }
        else
        {
            if(v)verbose("Consigo un archivo, imprimo su nombre.\n",foutput);
            printf("%*s- %s\n", indent, "", entry->d_name);
            fwrite(entry->d_name, 1, sizeof(entry->d_name), fp2);
        }
 
    }
    closedir(dir);
}