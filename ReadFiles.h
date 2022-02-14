#pragma once
class ReadFiles
{
public:
	//Cuenta los elementos de la lista de archivos
	static int CountLines(const char* TxtName);
	//Almacena la lista de archivos
	static void ReadF(const char* TxtName, char*** ListF, int n);
	//Obtiene los valores maximos de los arreglos
	static void GetMaxObjects(char** ListF, int n, int* Mx, int* My, int* Mz);
	//Obtiene los valores primarios del txt
	static void GetValObjects(char** ListF, int n, int* lenD, int* lenT, int* lenS);
	//Lectura de las instancias
	static void ReadInstances(const char* TxtName, int* lenD, int* lenT, int* lenC, int Olen, float*** MtxD, float*** MtxT, float*** MtxS, int*** MtxC, int** SolTxt, float* CtsTxtS);
};

