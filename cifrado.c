
unsigned char cshiftR(unsigned char x, int z) /*PARA CIFRAR*/
{ /*shift circular al caracter x, z veces a la derecha*/
	int n;
	if(z<=8)
		n = z;	
	else{
		n = z % 8; 
	}
	return (x >> n) | (x << (8 - n));
}


unsigned char cshiftL(unsigned char x, int y) /*PARA DESCIFRAR*/
{ /*shift circular al caracter x, y veces a la derecha*/
	int n;
	if(y<=8)
		n = y;	
	else{
		n = y % 8; 
	}
	return (x << n) | (x >> (8 - n));
}

void cifrarArchivo(FILE *fp, int n) /*opcion -z*/
{ /*recorre archivo caracter por caracter, cifrando cada uno*/
	char c_actual; /*el caracter donde estoy parado*/
	char c_cif;
	while ((c_actual = fgetc(fp)) != EOF) /*lee un caracter a la vez mientras no sea EOF*/
	{
		c_cif = cshiftR(c_actual,n); /*el valor de c_actual cifrado*/
		fseek(fp, ftell(fp)-1, SEEK_SET); /*devuelve el puntero un caracter para sobreescribir*/
		fputc(c_cif,fp); /*sobreescribe el cifrado sobre el descifrado*/
	}
}


void descifrarArchivo(FILE *fp, int n) /*opcion -y*/
{ /*recorre archivo caracter por caracter, descifrando cada uno*/
	char c_actual; /*el caracter donde estoy parado*/
	char c_desc; 
	while ((c_actual = fgetc(fp)) != EOF) /*lee un caracter a la vez mientras no sea EOF*/
	{
		c_desc = cshiftL(c_actual,n); /*el verdadero valor de c_actual*/
		fseek(fp, ftell(fp)-1, SEEK_SET); /*retrocede el puntero un caracter para sobreescribir*/
		fputc(c_desc,fp); /*sobreescribe el descifrado sobre el cifrado*/
	}
}