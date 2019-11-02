#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50

/*PARA CIFRAR*/
unsigned char cshiftR(unsigned char x, int m)
{ /*shift circular a la derecha m veces al caracter x*/
	int n;
	if(m<=8)
		n = m;	
	else{
		n = m % 8; 
	}
	return (x >> n) | (x << (8 - n));
}

/*PARA DESCIFRAR*/
unsigned char cshiftL(unsigned char x, int m)
{ /*shift circular a la izquierda m veces al caracter x*/
	int n;
	if(m<=8)
		n = m;	
	else{
		n = m % 8; 
	}
	return (x << n) | (x >> (8 - n));
}

void cifrarArchivo(FILE *fp, int n)
{ /*recorre fp caracter por caracter, cifrando cada uno (un 1B)*/
	char c_actual; /*el caracter donde estoy parado*/
	char c_cif;
	while ((c_actual = fgetc(fp)) != EOF) /*lee un caracter a la vez mientras no sea EOF*/
	{
		c_cif = cshiftR(c_actual,n); /*el valor de c_actual cifrado*/
		fseek(fp, ftell(fp) - 1, SEEK_SET); /*devuelve el puntero un caracter para sobreescribirlo*/
		fputc(c_cif,fp); /*sobreescritura del cifrado sobre el descifrado*/
	}
}


void descifrarArchivo(FILE *fp, int n)
{ /*recorre fp caracter por caracter, descifrando cada uno*/
	char c_actual; /*el caracter donde estoy parado*/
	char c_desc; 
	while ((c_actual = fgetc(fp)) != EOF) /*lee un caracter a la vez mientras no sea EOF*/
	{
		c_desc = cshiftL(c_actual,n); /*el verdadero valor de c_actual*/
		fseek(fp, ftell(fp) - 1, SEEK_SET); /*devuelve el puntero un caracter para sobreescribirlo*/
		fputc(c_desc,fp); /*sobreescritura del descifrado sobre el cifrado*/
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
	printf("FLAGS: z=%d y=%d\n",zflag,yflag);

	printf("Escoja la llave de cifrado: ");
	scanf("%d",&n);

	if (zflag)
		cifrarArchivo(fp,n);
	if (yflag)
		descifrarArchivo(fp,n);


	fclose(fp);
	return 0;
}