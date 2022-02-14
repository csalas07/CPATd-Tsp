#include "ReadFiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

#define Gmax(a,b) (a>b ? a : b)

//Cuenta los elementos de la lista de archivos
int ReadFiles::CountLines(const char* TxtName) {
	int count = 0;
	string line;
	ifstream inFile;
	inFile.open(TxtName);

	if (!inFile) {
		cout << "No se ha podido leer el archivo";
		exit(1); // terminate with error
	}

	while (getline(inFile, line)) {
		count++;
	}

	inFile.close();
	//cout << "Sum = " << count << endl; 
	return count;
}

//Almacena la lista de archivos
void ReadFiles::ReadF(const char* TxtName, char*** ListF, int n) {
	int i,j=0;

	//FILE* f = fopen(TxtName, "r");
	FILE* f;
	errno_t err;
	
	err = fopen_s(&f,TxtName, "r");

	if (err == 0) {
		for (i = 0; i < n; i++) {
			//fscanf(f,"%s", &(*ListF)[i][j]);
			fscanf_s(f, "%s", (*ListF)[i],30);
			//sscanf_s("%s", &*ListF[i][j]);
		}


		if (f) {
			err= fclose(f);

			if (err!=0) {
				exit(6);
			}

		}
	}else {
		exit(6);
	}
	
}

//Obtiene los valores maximos de los arreglos
void ReadFiles::GetMaxObjects(char** ListF, int n, int* Mx, int* My, int* Mz) {
	int i;
	int MaxX=0, MaxY=0, MaxZ=0;
	int MxX, MxY, MxZ;
	FILE* f;
	for (i = 0; i < n; i++){
		fopen_s(&f, ListF[i], "r");
		fscanf_s(f, "%i", &MxX, 1); //Max X
		fscanf_s(f, ",%i", &MxY, 1); //Max Y
		fscanf_s(f, ",%i", &MxZ, 1); //Max Z

		MaxX = Gmax(MaxX, MxX);
		MaxY = Gmax(MaxY, MxY);
		MaxZ = Gmax(MaxZ, MxZ);
		fclose(f);
	}
	*Mx = MaxX;
	*My = MaxY;
	*Mz = MaxZ;
}

//Obtiene los valores primarios del txt
void ReadFiles::GetValObjects(char** ListF, int n, int* lenD, int* lenT, int* lenS) {

	int i;
	int MlD = 0, MaxD = 0;
	int MlT = 0, MaxT = 0;
	int MlS = 0, MaxS = 0;
	char s[1];
	FILE* f;

	for (i = 0; i < n; i++) {
		fopen_s(&f, ListF[i], "r");
		fscanf_s(f, "%c:%i\n", s, 1, &MlD, 1); //Valor de N
		//fscanf_s(f, "%c:%i\n", s, 1, &(*lenD), 1); //Valor de N
		fscanf_s(f, "%c:%i\n", s, 10, &MlT, 1); //Valor de T
		//fscanf_s(f, "%c:%i\n", s, 10, &(*lenT), 1); //Valor de T
		fscanf_s(f, "%c:%i\n", s, 1, &MlS, 1); //Valor de C
		//fscanf_s(f, "%c:%i\n", s, 1, &(*lenS), 1); //Valor de C
		MaxD = Gmax(MlD, MaxD);
		MaxT = Gmax(MlT, MaxT);
		MaxS = Gmax(MlS, MaxS);
		fclose(f);
	}
	*lenD = MaxD;
	*lenT = MaxT;
	*lenS = MaxS;
}

//Lectura de otra instancia
void ReadFiles::ReadInstances(const char* TxtName, int* lenD, int* lenT, int* lenC, int Olen, float*** MtxD, float*** MtxT, float*** MtxS, int*** MtxC, int** SolTxt, float* CtsTxtS) {
	//void ReadFiles::ReadInstanceTwo(const char* TxtName, int lenD, int lenT, int lenC, int Olen, float*** MtxD, float*** MtxT, float*** MtxS, int*** MtxC, int** SolTxt, float* CtsTxtS){
	int i, j, k;
	char s[10];
	FILE* f;

	fopen_s(&f, TxtName, "r");

	//fscanf_s(f, "%*[^\n]\n");
	fscanf_s(f, "%c:%i\n", s, 1, &(*lenD), 1); //Valor de N
	//fscanf_s(f, "%*[^\n]\n");
	fscanf_s(f, "%c:%i\n", s, 1, &(*lenT), 1); //Valor de T
	//fscanf_s(f, "%*[^\n]\n");
	fscanf_s(f, "%c:%i\n", s, 1, &(*lenC), 1); //Valor de S

	fscanf_s(f, "%*[^\n]\n");

	for (i = 0; i < *lenD; i++) {
		//for (i = 0; i < lenD + Olen; i++){
		for (j = 0; j < *lenD; j++) {
			//for (j = 0; j < lenD + Olen; j++) {
			fscanf_s(f, "%f", &(*MtxD)[i][j], 1);
			//cout << (*MtxD)[i][j] << ",";
		}
		//printf("\n");
	}
	//printf("\n");

	fscanf_s(f, "%s", s, 10);

	for (i = 0; i < *lenT; i++) {
		for (j = 0; j < Olen; j++) {
			fscanf_s(f, "%f", &(*MtxT)[i][j], 1);
			//cout << (*MtxT)[i][j] << ",";
		}
		//printf("\n");
	}

	//printf("\n");

	fscanf_s(f, "%s", s, 10);

	for (i = 0; i < *lenC; i++) {
		for (j = 0; j < *lenC; j++) {
			fscanf_s(f, "%f", &(*MtxS)[i][j], 1);
			//cout << (*MtxS)[i][j] << ",";
		}
		//printf("\n");
	}

	//printf("\n");

	fscanf_s(f, "%s", s, 10);

	for (i = 0; i < *lenD; i++) {
		//for (i = 0; i < lenD + Olen; i++) {
		fscanf_s(f, "%i", &(*SolTxt)[i], 1);
		//cout << (*SolTxt)[i] << ",";
	}
	//printf("\n");

	fscanf_s(f, "%s", s, 10);

	fscanf_s(f, "%f", &(*CtsTxtS), 1);

	//cout << *CtsTxtS << endl;
	//printf("\n");

	fscanf_s(f, "%s", s, 10);

	fscanf_s(f, "%s", s, 10);

	fscanf_s(f, "%s", s, 10);

	for (i = 0; i < *lenD; i++) {
		//for (i = 0; i < lenD + Olen; i++) {
		for (j = 0; j < *lenD; j++) {
			//for (j = 0; j < lenD + Olen; j++) {
			fscanf_s(f, "%i", &(*MtxC)[i][j], 1);
			//cout << (*MtxC)[i][j] << ",";
		}
		//printf("\n");
	}

	//Transformacion
	for (i = 0; i < *lenD; i++) {
		//for (i = 0; i < lenD + Olen; i++) {
		for (j = 0; j < *lenD; j++) {
			//for (j = 0; j < lenD + Olen; j++) {
			(*MtxC)[i][j] = (*MtxC)[i][j] - 1;
		}
	}

	fclose(f);
}