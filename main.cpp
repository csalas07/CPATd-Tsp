#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
#include <math.h>
#include <cmath>

#include "Memory.h"
#include "ReadFiles.h"

#define getRandom(min,max) (rand() % (int)(max-min)+min)
#define getRandomF(min, max) (((max - min) * ((float)rand() / RAND_MAX)) + min) 
#define maxValues(a,b) (a>b ? a : b)
#define minValues(a,b) (a<b ? a : b)

/// <summary>
/// Enum para el arreglo de las LCS.
/// en = habilitado, 0 = No habilitado, 1 = Habilitado.
/// set = usado, 0 = No listo, 1 = Listo. 
/// </summary>
enum lcs {
	en,
	set,
	ad,
	c,
};

/// <summary>
/// Enum para el arreglo TestSolutions
/// </summary>
enum TS
{
	Tv,
	ObV
};


#pragma region Variables y matrices de instancias
const int Olen = 2;
float** MtxD, ** MtxT, ** MtxS; //MtxD = Matriz de distancias, MtxT = Matriz de tiempos, MtxS = Matriz de velocidades
int n, ** MtxC, * SolTxt; //n=tamaño de la memoria de costos, J y arr, MtxC = Matriz de clusters, SolTxt = Solucion que tiene el txt
int MlenD, MlenT, MlenS; //lenD = tamaño de la matriz de distancias, lenT = tamaño del arreglo de tiempos, lenS = tamaño del arreglo de la matriz de velocidades
int lenD, lenT, lenS; //lenD = tamaño de la matriz de distancias, lenT = tamaño del arreglo de tiempos, lenS = tamaño del arreglo de la matriz de velocidades
int m;
float CtsTxt;
#pragma endregion

#pragma region Variables para TDTSP
int idxC;
float distance, t, ArT, OldArt, vSpeed, *cTot;
#pragma endregion

#pragma region Variables y Matrices para GRASP
float **lc;
float *beta;
int **perms;
int ***records; //3D para el registro de la memoria compartida 
#pragma endregion

#pragma region Variables y Matrices para PCELL
//int *sol1, *sol2, *sol3, *sol4, *sol5, **sol; Propuesta de arreglos o quiza un arreglo 2D
//float Lb1, Lb2, Lb3, Lb4, Lb5; Propuesta de variables
#pragma endregion

#pragma region Declaracion de Funciones
void GRASP(int** perm, float*** lc);
float CalcOv(int* perm);
int Roulette(int nRow, float*** Lc);
float CalcTime(int jG, int j2);
float GetSpeed(int clst, float** ArTime, float** ArSpeed, float Cutime, int MTm);
void printValues();
#pragma endregion

int main() {

#pragma region Variables Internas
	srand(127);
	int Nfiles, i, l;
	char** Flist; //Lista de archivos
	//char* Fname;//Nombre del archivo de texto de la salida de resultados
	long start, end;//Tiempos inicial y final
	double totalTime;//Tiempo total
	float lowerBound;//Indice bajo sacado por funciones
#pragma endregion

#pragma region LowerBound
	float ov, bestOv = FLT_MAX;
	int iterSinMejora = 10;
#pragma endregion

#pragma region Lectura de lista de archivos
	Nfiles = ReadFiles::CountLines("ListF.txt");	//Conteo de archivos
	Flist = Memory::ReserveBiVectorChar(Nfiles, 100); //Memoria para la lista de archivos
	ReadFiles::ReadF("ListF.txt", &Flist, Nfiles); //Lectura de la lista de archivos
	ReadFiles::GetValObjects(Flist, Nfiles, &MlenD, &MlenT, &MlenS); //lectura de los primeros valores
#pragma endregion

#pragma region Memoria de datos
	MtxD = Memory::ReseveBiVectorflt(MlenD, MlenD); //Memoria matriz de distancias
	MtxT = Memory::ReseveBiVectorflt(MlenT, Olen); //Memoria Matriz de tiempos
	MtxS = Memory::ReseveBiVectorflt(MlenS, MlenS); //Memoria Matriz de velocidades
	MtxC = Memory::ReseveBiVectorInt(MlenD, MlenD); //Matriz de clusters
	SolTxt = Memory::ReserveVectorInt(MlenD); //Matriz de Solucion
	records = Memory::ReseveTriVectorInt(MlenD, MlenD, MlenD);
	perms = Memory::ReseveBiVectorInt(5, MlenD);
	//perms = new int[MlenD]; //Generar memoria para permutacion
	lc = Memory::ReseveBiVectorflt(MlenD, 4); //Generar memoria para lc
	beta = Memory::ReserveVectorflt(5);
	cTot = Memory::ReserveVectorflt(5);
	n = MlenD;
#pragma endregion

#pragma region Proceso de instancias
	printf("Procesando Instancias \n\n");
	for (l = 0; l < Nfiles; l++) {
		ReadFiles::ReadInstances(Flist[l], &lenD, &lenT, &lenS, Olen, &MtxD, &MtxT, &MtxS, &MtxC, &SolTxt, &CtsTxt);
		n = lenD;
		start = clock(); //Inicio del Reloj
		//printValues();


		do { //Genera el LowerBound
			for (i = 0; i < n; i++) {
				perms[0][i] = -1;
				lc[i][lcs::en] = 1;
				lc[i][lcs::set] = 0;
				lc[i][lcs::ad] = 0;
				lc[i][lcs::c] = 0;
			}
			cTot[0] = 0;
			iterSinMejora--;
			GRASP(&perms[0], &lc); //Generar una permutación con GRASP
			printf("Ctot  %f\n", cTot[0]);
			ov = CalcOv(perms[0]);	//Calculo de la funcion objetivo
			if (ov < bestOv){
				bestOv = ov;
				iterSinMejora = 10;
			}
			//printf("Ctot  %f\n", ov);
		} while (iterSinMejora > 0);

		end = clock(); //Fin del Reloj
		totalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
		printf("Termine Archivo  %s\n", Flist[l]);
		printf("El tiempo fue %f\n", totalTime);
		printf("El valor objetivo final fue: %f\n", bestOv);
		//printf("El valor bt final fue: %f\n", bt);
		printf("\n");
	}
#pragma endregion
	printf("Ya acabe \n\n");
	_getch();
}

#pragma region Lista de Funciones

void GRASP(int **perms, float ***lc) {
	int miss = n, i = 0, Valrand, prev, nextV, j = 0;
	float Vmax, Vmin;
	float limit;

	beta[0] = 0.30;
	//Valrand = getRandom(1, n);
	(*perms)[0] = 0; //Primer Valor 0
	(*lc)[0][lcs::en] = 0; //Ya fue seleccionado
	(*lc)[0][lcs::set] = 1;
	(*lc)[0][lcs::ad] = 0;
	(*lc)[0][lcs::c] = 0;
	miss--;
	prev = (*perms)[0];

	do
	{
		Vmax = FLT_MIN;
		Vmin = FLT_MAX;
		for (i = 0; i < n; i++) { //Recorrido de rutas

			if ((*lc)[i][lcs::set] == 0) { //Verificamos si el elemento se encuentra con valor 1
				(*lc)[i][lcs::en] = 1;
				(*lc)[i][lcs::ad] = CalcTime(prev, i);
				Vmin = minValues((*lc)[i][lcs::ad], Vmin);
				Vmax = maxValues((*lc)[i][lcs::ad], Vmax);
			}
		}

		limit = Vmin + (beta[0]* (Vmax - Vmin));

		for (i = 0; i < n; i++) { //Seleccion de elementos para el LCR
			if ((*lc)[i][lcs::en] == 1) {
				if ((*lc)[i][lcs::ad] > limit) {
					(*lc)[i][lcs::en] = 0;
				}
			}
		}

		nextV = Roulette(n, lc);//Seleccion aleatoria del siguiente elemento
		j++;
		(*perms)[j] = nextV;
		(*lc)[nextV][lcs::en] = 0;
		(*lc)[nextV][lcs::set] = 1;
		//Revisar la actualizacion de Ctot, el cual debe almacenarlo ad
		//printf("Ctot  %f\n", cTot);
		cTot[0] += (*lc)[nextV][lcs::ad];
		prev = (*perms)[j];
		miss--;
	} while (miss > 0);
}

float CalcOv(int* perm)
{
	int i = 0; //index de la ruta (permutación)
	m = 0; //index del cluster de tiempos arriba/abajo
	t = 0; //tiempo inicial

	for (i = 0; i < n; i++)
	{
		idxC = MtxC[perm[i]][perm[(i + 1) % (n)]]; //valor del cluster // Hay que hacerle funcion 24/06/2021
		distance = MtxD[perm[i]][perm[(i + 1) % (n)]]; //Calculamos distancia del punto i a j
		vSpeed = GetSpeed(idxC, MtxT, MtxS, t, lenT); //obtenemos que velocidad pertenece
		ArT = t + (distance / vSpeed); //Calculamos el arrival time

		while (ArT > MtxT[m][1])
		{ //Inicio del ciclo
			distance = distance - (vSpeed * (MtxT[m][1] - t)); //Calculamos distancias
			t = MtxT[m][1]; //Nuevo tiempo
			vSpeed = GetSpeed(idxC, MtxT, MtxS, t, lenT); //obtenemos que velocidad pertenece
			ArT = t + (distance / vSpeed); //Calculamos el arrival time
			if (m < lenT - 1) {
				m++; //incremento de la matriz de tiempos
			}
			else
			{
				break;
			}
		}
		t = ArT;
	}
	return (ArT);
}

int Roulette(int nRow, float*** Lc) {
	int i;
	float Vmin, Vmax, Valmin, val, rdm = 0, sum;

	//Maximos y minimos
	Valmin = 1;
	Vmin = FLT_MAX;
	Vmax = FLT_MIN;
	for (i = 0; i < nRow; i++) {
		if ((*Lc)[i][lcs::en] == 1) {
			//Encontrando el valor minimo
			Vmin = minValues((*Lc)[i][lcs::ad], Vmin);
			//Encontrando el valor maximo
			Vmax = maxValues((*Lc)[i][lcs::ad], Vmax);
		}
	}

	//Conversion de valores menores a mayores y viceversa
	//Sumatoria
	val = 0;
	sum = Vmax + Vmin;
	for (i = 0; i < nRow; i++) {
		if ((*Lc)[i][lcs::en] == 1) {
			(*Lc)[i][lcs::c] = sum - abs((*Lc)[i][lcs::ad]);
			val = val + (*Lc)[i][lcs::c];
		}
	}

	do {
		//Generacion de numeros aleatorios
		do {
			rdm = getRandomF(0, val);
		} while (rdm == 0);

		//Resta de valores nuevos con el aleatorio
		for (i = 0; i < nRow; i++) {
			if ((*Lc)[i][lcs::en] == 1) {
				rdm = rdm - (*Lc)[i][lcs::c];
				if (rdm <= 0) {
					break;
				}
			}
		}
	} while (rdm > 0);
	return i;
}

float CalcTime(int jG, int j2)
{
	idxC = MtxC[jG][j2]; //valor del cluster.... Funcion realizada 24/06/2021
	distance = MtxD[jG][j2]; //Calculamos distancia del punto i a j
	//distanciatotal += distance;
	t = cTot[0];
	vSpeed = GetSpeed(idxC, MtxT, MtxS, t, lenT); //obtenemos que velocidad pertenece
	OldArt = t;
	ArT = t + (distance / vSpeed); //Calculamos el arrival time

	//ArT = GetTimes(ArT, MtxT, MtxS, distance, t, &m, lenT, idxC, vSpeed);

	//Se tiene que revisar para meter las variables 24/06/202
	while (ArT > MtxT[m][1]) { //Inicio del ciclo
		distance = distance - (vSpeed * (MtxT[m][1] - t)); //Calculamos distancias
		t = MtxT[m][1]; //Nuevo tiempo
		vSpeed = GetSpeed(idxC, MtxT, MtxS, t, lenT); //obtenemos que velocidad pertenece
		ArT = t + (distance / vSpeed); //Calculamos el arrival time
		//costos[i] = ArT;
		if (m < lenT - 1) {
			m++; //incremento de la matriz de tiempos
		}
		else
		{
			break;
		}
	}
	return (ArT - OldArt);
}

float GetSpeed(int clst, float** ArTime, float** ArSpeed, float Cutime, int MTm) {

	int i;

	for (i = 0; i < MTm; i++) {
		if ((Cutime >= ArTime[i][0]) && (Cutime < ArTime[i][1])) {
			return ArSpeed[i][clst];
		}
	}

	return 0.0001;
}

void printValues() {
	int i, j;
	std::cout << "Matriz de distancias D" << std::endl;
	printf("\n");

	for (i = 0; i < lenD; i++) {
		for (j = 0; j < lenD; j++)
		{
			std::cout << (MtxD)[i][j] << ",";
		}
		printf("\n");
	}
	printf("\n");

	std::cout << "Matriz de tiempos D" << std::endl;
	printf("\n");

	for (i = 0; i < lenT; i++) {
		for (j = 0; j < Olen; j++) {
			std::cout << MtxT[i][j] << ",";
		}
		printf("\n");
	}
	printf("\n");

	std::cout << "Matriz de velocidades S" << std::endl;
	printf("\n");

	for (i = 0; i < lenS; i++) {
		for (j = 0; j < lenS; j++) {
			std::cout << MtxS[i][j] << ",";
		}
		printf("\n");
	}
	printf("\n");


	std::cout << "Matriz SolTxt" << std::endl;
	printf("\n");

	for (i = 0; i < lenD; i++) {
		std::cout << SolTxt[i] << ",";
	}
	printf("\n");
	printf("\n");

	std::cout << "Valor CtsTxt" << std::endl;
	printf("\n");

	std::cout << CtsTxt << std::endl;
	printf("\n");


	std::cout << "Matriz de clusters C" << std::endl;
	printf("\n");

	for (i = 0; i < lenD; i++) {
		for (j = 0; j < lenD; j++) {
			std::cout << MtxC[i][j] << ",";
		}
		printf("\n");
	}
	printf("\n");
}
#pragma endregion