#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50

/*PARA CIFRAR*/
unsigned char cshiftR(unsigned char x, int z) 
{ 
	int n;
	if(z<=8)
		n = z;	
	/* Si z>8, calculamos z%8 para saber cuántas veces realmente se rota,
	* pues rotar el byte por múltiplos de 8 resulta en el mismo byte */
	else{
		n = z % 8; 
	}
	return (x >> n) | (x << (8 - n));
}
/*PARA DESCIFRAR*/
unsigned char cshiftL(unsigned char x, int y)
{
	int n;
	if(y<=8)
		n = y;	
	/* Si y>8, calculamos y%8 para saber cuántas veces realmente se rota,
	* pues rotar el byte por múltiplos de 8 resulta en el mismo byte */	
	else{
		n = y % 8; 
	}
	return (x << n) | (x >> (8 - n));
}


int encryptArchive(FILE *fmytar, int n)
{
	char c_actual;	/* Variable que almacena el caracter donde estoy parado */
	char c_cif;		/* Variable que almacena el nuevo caracter cifrado */
	char bandera_cif[10]; /* Almacena la bandera de cifrado del archivo */

	fscanf(fmytar,"%s",bandera_cif); /* Recibe la bandera del archivo */

	if (strcmp(bandera_cif,"ENCRYPT:1") == 0)
	{
		printf("Error: ya el archivo esta cifrado.\n");
		return 0;
	}

	fseek(fmytar,8,SEEK_SET);
	fputc('1',fmytar); /* Actualiza la bandera para indicar que se cifró*/
	fseek(fmytar, 10, SEEK_SET);
	/* Lee un caracter a la vez mientras no sea EOF */
	while ((c_actual = fgetc(fmytar)) != EOF) 
	{
		c_cif = cshiftR(c_actual,n); /* Se almacena  el caracter cifrado */
		fseek(fmytar, ftell(fmytar)-1, SEEK_SET); /* Retrocede el puntero un
													caracter para sobreescribir*/
		fputc(c_cif,fmytar); /* Sobreescribe el cifrado sobre el descifrado */
	}
	return 1;
}


int decryptArchive(FILE *fmytar, int n)
{
	char c_actual; /* Variable que almacena el caracter donde estoy parado */
	char c_desc;   /* Variable que almacena el nuevo caracter descifrado */
	char bandera_cif[10]; /* Almacena la bandera de cifrado del archivo */
	char *bandera_key; /* Almacena "KEY", si se consigue "KEY" en el archivo*/
	char *buff; /* Almacena 5 bytes del archivo para buscar "KEY" luego*/

	fscanf(fmytar,"%s",bandera_cif); /* Recibe la bandera del archivo */

	if (strcmp(bandera_cif,"ENCRYPT:0") == 0)
	{
		printf("Error: ya el archivo esta descifrado.\n");
		return 0;
	}

	fseek(fmytar,8,SEEK_SET);
	fputc('0',fmytar); /* Actualiza la bandera para indicar que se descifró*/
	fseek(fmytar, 10, SEEK_SET);
	/* Lee un caracter a la vez mientras no sea EOF */
	while ((c_actual = fgetc(fmytar)) != EOF)
	{
		c_desc = cshiftL(c_actual,n); /* Se almacena  el caracter descifrado */
		fseek(fmytar, ftell(fmytar)-1, SEEK_SET); /* Retrocede el puntero un
													caracter para sobreescribir */
		fputc(c_desc,fmytar); /* Sobreescribe el descifrado sobre el cifrado */
	}

	bandera_key = "KEY";

	/* Mueve el puntero a un punto después de la bandera de cifrado */
	fseek(fmytar,10,SEEK_SET);
	fgets(buff,5,fmytar); /* almacena los siguientes 5 chars, buscamos "KEY" */

	if (strstr(buff,bandera_key)) /* Si se consigue "KEY" en buff, bien */
	{
		return 1;
	}
	else
	{
		/* si no se consigue "KEY" la rotación de bits no descifró el archivo */
		printf("Error: los bits rotados no coinciden con los del cifrado.\n");
		return 0;
	}
}

int main ()
{
	char fname[MAX]; /*archivo sobre el cual trabajaremos*/
	FILE *fp; /*puntero al archivo anterior*/
	int n; /*llave de cifrado o cantidad de shifts sobre cada caracter*/
	int zflag; /*indica que se desa cifrar*/
	int yflag; /*indica que se desea descifrar*/
	char opc; /*aqui se almacena la opcion de cifrar o la de descifrar*/
	

	printf("Hola. Con que archivo trabajaremos? ");
	gets(fname); 
	fp = fopen(fname, "r+");
	if (fp == NULL)
		{
			printf("Error: no se ha creado el archivo %s\n",fname);
			exit(-1); /*Si el archivo no existe, termina el programa*/
		}

	zflag = 0;
	yflag = 0;
	printf("Desea cifrar [z] o descifrar [y]? ");
	scanf("%c",&opc);
	if (opc == 'z' || opc == 'Z')
		zflag = 1;
	else if (opc == 'y' || opc == 'Y')
		yflag = 1;
	else
	{
		printf("Error: opcion desconocida.\nSolo 'z'/'Z' y 'y'/'Y' son validos.\n");
		exit(-1); /*Si se escoge un caracter invalido, termina el programa*/
	}
	/*printf("FLAGS: z=%d y=%d\n",zflag,yflag);*/

	printf("Escoja la llave de cifrado: ");
	scanf("%d",&n);

	if (zflag)
		if (encryptArchive(fp,n))
			printf("Cifrado fabuloso!");
		else
		{
			printf("Cifrado desastroso!");
			fclose(fp);
			return -1;
		}
	if (yflag)
		if (decryptArchive(fp,n)==1)
			printf("Descifrado asombroso!");
		else
		{
			printf("Descifrado espantoso!");
			fclose(fp);
			return -1;
		}

	fclose(fp);
	return 0;
}