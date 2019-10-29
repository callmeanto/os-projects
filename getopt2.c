#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]){

  char *fvalue = NULL;
  int cflag = 0;
  int nflag = 0;
  int tflag = 0;
  char *ovalue = NULL;
  int zvalue = 0;
  int yvalue = 0;
  int xflag = 0;
  int vflag = 0;
  char *vvalue = NULL;
  char *svalue = NULL;
  int index;
  int opc;

  opterr = 0;

  while ((opc = getopt (argc, argv, "f:cnto:z:y:xv:s:")) != -1)
    switch (opc)
      {
      case 'f': //especifica el tar a usar
        fvalue = optarg;
        break;
      case 'c': //permite crear el tar
        cflag = 1;
        break;
      case 'n': //ignora archivos que no sean regulares o directorios
        nflag = 1;
        break;
      case 't': //muestra el contenido del tar
        tflag = 1;
        break;
      case 'o': //salida de una extracción
        ovalue = optarg;
        break;
      case 'z': //cifra
        zvalue = atoi(optarg);
        break;
      case 'y': //descifra
        yvalue = atoi(optarg);
        break;
      case 'x': //extrae todo
        xflag = 1;
        break;
      case 's': //extrae un archivo especifico del tar
        svalue = optarg;
        break;
      case 'v': //verbosity
        vvalue = optarg;
        vflag = 1;        
        break;
      case '?':
        if (optopt == 'f')
          fprintf (stderr, "Opcion -%c necesita un argumento.\n", optopt);
        else if (optopt == 'o')
          fprintf (stderr, "Opcion -%c necesita un argumento.\n", optopt);
        else if (optopt == 'z')
          fprintf (stderr, "Opcion -%c necesita un argumento.\n", optopt);
        else if (optopt == 'y')
          fprintf (stderr, "Opcion -%c necesita un argumento.\n", optopt);
        else if (optopt == 's')
          fprintf (stderr, "Opcion -%c necesita un argumento.\n", optopt);
        else if (optopt == 'v'){
          vflag = 1;
          break;}
        else if (isprint (optopt))
          fprintf (stderr, "Opcion desconocida '-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character '\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort();
      }


  printf("fvalue = %s, cflag = %d, nflag = %d, tflag = %d, ovalue = %s,\nzvalue = %d, yvalue = %d, xflag = %d, svalue = %s, vflag = %d, vvalue = %s\n",
          fvalue, cflag, nflag, tflag, ovalue, zvalue, yvalue, xflag, svalue, vflag, vvalue);

  for (index = optind; index < argc; index++)
    printf ("Argumento sin opcion: %s\n", argv[index]);
  return 0;
}