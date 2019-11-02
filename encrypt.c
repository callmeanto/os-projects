/*
* Funcion: Rotación a la derecha
* --------------------
* Función utilizada para el cifrado de archivos de texto. Recibe un caracter
* (1 byte) y luego procede a rotarlo a la derecha 'z' veces. Para conseguir
* el byte rotado (shift circular), shifteamos el byte a la derecha 'z' veces
* con el operador >> y para setear los (8-z) bits del lado izquierdo que 
* están en 0 utilizamos un operador OR con el shift a la izquierda del mismo
* byte (8-z) veces. Esta función retorna el byte rotado a la derecha.
*
* x: caracter de un archivo que será cifrado
* z: cantidad de veces que se rotan los bits del byte a la derecha
*/
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

/*
* Funcion: Rotación a la izquierda
* --------------------
* Función utilizada para el descifrado de archivos de texto. Recibe un caracter
* (1 byte) y luego procede a rotarlo a la izquierda 'y' veces. Para conseguir
* el byte rotado (shift circular), shifteamos el byte a la izquierda 'y' veces
* con el operador << y para setear los (8-y) bits del lado derecho que
* están en 0 utilizamos un operador OR con el shift a la derecha del mismo
* byte (8-z) veces. Esta función retorna el byte rotado a la izquierda.
* 
* x: caracter de un archivo que será descifrado
* z: cantidad de veces que se rotan los bits del byte a la izquierda
*/
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

/*
* Funcion: Cifrado del archivo tar
* --------------------
* Si la opción -z fue marcada en la entrada, esta función cifra el archivo mytar
* que se debió crear previamente. El cifrado consiste en rotar los bits de cada
* byte a la derecha n veces. Esto lo logra leyendo los caracteres del archivo
* uno por uno, rotándolos a la derecha y sustituyendo el caracter original por
* su versión rotada. Esto funciona pues sabemos que el archivo mytar es de tipo
* texto, por lo que cada caracter es un solo byte. No retorna nada (tipo void).
*
* fmytar: apuntador al archivo mytar 
* n: cantidad de veces que se rotan los bits de cada byte a la derecha
*/
void encryptArchive(FILE *fmytar, int n)
{
	char c_actual;	/* Variable que almacena el caracter donde estoy parado */
	char c_cif;		/* Variable que almacena el nuevo caracter cifrado */

	/* Lee un caracter a la vez mientras no sea EOF */
	while ((c_actual = fgetc(fmytar)) != EOF) 
	{
		c_cif = cshiftR(c_actual,n); /* Se almacena  el caracter cifrado */
		fseek(fmytar, ftell(fmytar)-1, SEEK_SET); /* Retrocede el puntero un
													caracter para sobreescribir*/
		fputc(c_cif,fmytar); /* Sobreescribe el cifrado sobre el descifrado */
	}
}

/*
* Funcion: Cifrado del archivo tar
* --------------------
* Si la opción -y fue marcada en la entrada, esta función descifra el archivo 
* mytar antes de extraer su contenido total o parcialmente. El descifrado
* consiste en rotar los bits de cada byte a la izquierda n veces. Esto lo logra 
* leyendo los caracteres del archivo uno por uno, rotándolos a la izquierda y 
* sustituyendo el caracter cifrado por su versión descifrada. Esto funciona
* pues sabemos que el archivo mytar es de tipo texto, por lo que cada caracter
* es un solo byte. Si la cantidad de rotaciones no coincide con las rotaciones
* del cifrado, el archivo seguirá cifrado. No retorna nada (tipo void).
*
* fmytar: apuntador al archivo mytar 
* n: cantidad de veces que se rotan los bits de cada byte a la izquierda
*/
void decryptArchive(FILE *fmytar, int n)
{
	char c_actual; /* Variable que almacena el caracter donde estoy parado */
	char c_desc;   /* Variable que almacena el nuevo caracter descifrado */

	/* Lee un caracter a la vez mientras no sea EOF */
	while ((c_actual = fgetc(fmytar)) != EOF)
	{
		c_desc = cshiftL(c_actual,n); /* Se almacena  el caracter descifrado */
		fseek(fmytar, ftell(fmytar)-1, SEEK_SET); /* Retrocede el puntero un
													caracter para sobreescribir */
		fputc(c_desc,fmytar); /* Sobreescribe el descifrado sobre el cifrado */
	}
}