/*
* Encabezado de libreria de cifrar archivos 
* --------------------
* Autores: Antonella Requena 15-11196
*          Ezequiel Zabala 14-11160
*
* Ultima fecha de modificacion: 02/11/2019
*/

/* Definiciones */
#ifndef ENCRYPT_H_
#define ENCRYPT_H_


/* ************** Declaracion de Funciones *************** */
unsigned char cshiftR(unsigned char x, int z);

unsigned char cshiftL(unsigned char x, int y);

int encryptArchive(FILE *fmytar, int n);

int decryptArchive(FILE *fmytar, int n);


#endif
