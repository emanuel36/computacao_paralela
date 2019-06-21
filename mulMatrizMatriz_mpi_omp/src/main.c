#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "mpi.h"
#include "matriz.h"

int main(int argc, char *argv[]) {
	int numtasks, rank, len, rc; 
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		char *nomeArquivoMatriz1 = argv[1];
		char *nomeArquivoMatriz2 = argv[2];
		char *nomeArquivoResultado = argv[3];

		FILE *arquivoMatriz1 = fopen(nomeArquivoMatriz1, "r");
		FILE *arquivoMatriz2 = fopen(nomeArquivoMatriz2, "r");
		if ((arquivoMatriz1 == NULL) || (arquivoMatriz2 == NULL)) {
			printf("Erro ao abrir os arquivos.\n");
			return 1;
		}

		Matriz matriz1Aux = lerMatriz(arquivoMatriz1);
		Matriz matriz2Aux = lerMatriz(arquivoMatriz2);

		//salvando matrizes na memória
		int numLinhas1, numColunas1, numLinhas2, numColunas2;

		numLinhas1 = matriz1Aux.n;
		numColunas1 = matriz1Aux.m;
		double **matriz1 = (double **) malloc(numLinhas1 * sizeof(double *)); 
		for (int i = 0; i < numLinhas1; i++){
			matriz1[i] = (double *) malloc(numColunas1 * sizeof(double)); 
		}

		numLinhas2 = matriz2Aux.n;
		numColunas2 = matriz2Aux.m;
		double **matriz2 = (double **) malloc(numLinhas2 * sizeof(double *)); 
		for (int i = 0; i < numLinhas2; i++){
			matriz2[i] = (double *) malloc(numColunas2 * sizeof(double)); 
		}

		if(numColunas2 != numLinhas1){
			printf("Matrizes imcompatíveis!\n");
			return 1;
		}

		for(int i = 0; i < numLinhas1; i++){
			for(int j = 0; j < numColunas1; j++){
				matriz1[i][j] = matriz1Aux.data[i * matriz1Aux.m + j];
			}
		}

		for(int i = 0; i < numLinhas2; i++){
			for(int j = 0; j < numColunas2; j++){
				matriz2[i][j] = matriz2Aux.data[i * matriz2Aux.m + j];
			}
		}

		//Enviando número de linhas da matriz 2 para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&numLinhas2, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando número de colunas da matriz 2 para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&numColunas2, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando matriz 2 para os processos
		double linha2[numColunas2];
		for(int i = 1; i < numtasks; i++){
			for(int j = 0; j < numLinhas2; j++){
				for(int z = 0; z < numColunas2; z++){
					linha2[z] = matriz2[j][z];
				}
				MPI_Send(linha2, numColunas2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
			}
		}

		//Calculando quantidade de trabalho local
		int procLocal = numLinhas1/numtasks;

		//Enviando processamento local para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&procLocal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando número de linhas da matriz 1 para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&numLinhas1, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando número de colunas da matriz 1 para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&numColunas1, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando partes da matriz para os processos	
		double linha1[numColunas1];
		int inicioV, fimV;
		for(int i = 1; i < numtasks; i++){
			inicioV = procLocal * i; 
			fimV = inicioV + procLocal;
			for(int j = inicioV; j < fimV; j++){
				for(int z = 0; z < numColunas1; z++){
					linha1[z] = matriz1[j][z];
				}
				MPI_Send(linha1, numColunas1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
			}
		}

		//alocando espaço para a matriz resultado
		int numLinhas3 = numColunas1;
		int numColunas3 = numLinhas2;

		double **matrizResultado = (double **) malloc(numLinhas3 * sizeof(double *)); 
		for (int i = 0; i < numLinhas3; i++){
			matrizResultado[i] = (double *) malloc(numColunas3 * sizeof(double)); 
		}

		for(int i = 0; i < numLinhas3; i++){
			for(int j = 0; j < numColunas3 - 1; j++){
				matrizResultado[i][j] = 0.0;
			}
		}

	    // Realizando cálculo
		int i, j, k;
		#pragma omp parallel for shared(procLocal, numLinhas1, numColunas1, matrizResultado, matriz1, matriz2) private(i, j, k) schedule(static)
		for (i = 0; i < procLocal; i++){
			for (j = 0; j < numLinhas1; j++){
				for (k = 0; k < numColunas1; k++){
					matrizResultado[i][j] = matrizResultado[i][j] + (matriz1[i][k] * matriz2[k][j]); 
				}
			}
		}

		//Recebe contribuições dos processos
		double linha[numColunas3];
		for(int i = 1; i < numtasks; i++){
			for(int j = 0; j < procLocal; j++){
				MPI_Recv(linha, numColunas3, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for(int k = 0; k < numColunas3; k++){
					matrizResultado[i * procLocal + j][k] = linha[k];
				}
			}
		}

		//Armazena o resultado no arquivo.
		FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
		fprintf(arquivoResultado, "%d\n", numLinhas3);
		fprintf(arquivoResultado, "%d\n", numColunas3);
		for(int i = 0; i < numLinhas3; i ++){
			for(int j = 0; j < (numColunas3 - 1); j++){
				fprintf(arquivoResultado, "%.4f", matrizResultado[i][j]);
				if(j < (numColunas3 - 2)){
					fprintf(arquivoResultado, ":");
				}
			}
			fprintf(arquivoResultado, "\n");
		}

	}else{
		int numLinhas1, numColunas1, numLinhas2, numColunas2, procLocal;

		//Recebe número de linhas e colunas da matriz 2
		MPI_Recv(&numLinhas2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&numColunas2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Aloca espaço para a matriz 2
		double **matriz2 = (double **) malloc(numLinhas2 * sizeof(double *)); 
		for (int i = 0; i < numLinhas2; i++){
			matriz2[i] = (double *) malloc(numColunas2 * sizeof(double)); 
		}

		//Recebe matriz 2
		double vetorAux2[numColunas2];
		for(int i = 0; i < numLinhas2; i++){
			MPI_Recv(&vetorAux2, numColunas2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < numColunas2; j++){
				matriz2[i][j] = vetorAux2[j];
			}
		}

		//Recebendo processamento local
		MPI_Recv(&procLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Recebe número de colunas e linhas da matriz 1
		MPI_Recv(&numColunas1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&numLinhas1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Aloca espaço para a matriz 1
		double **matriz1 = (double **) malloc(procLocal * sizeof(double *)); 
		for (int i = 0; i < procLocal; i++){
			matriz1[i] = (double *) malloc(numColunas1 * sizeof(double)); 
		}

		//Recebendo partes da matriz 1
		double vetorAux1[numColunas1];
		for(int i = 0; i < procLocal; i++){
			MPI_Recv(&vetorAux1, numColunas1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < numColunas1; j++){
				matriz1[i][j] = vetorAux1[j];
			}
		}

		//alocando espaço para a matriz resultado
		int numLinhas3 = procLocal;
		int numColunas3 = numLinhas2;

		double **matrizResultado = (double **) malloc(numLinhas3 * sizeof(double *)); 
		for (int i = 0; i < numLinhas3; i++){
			matrizResultado[i] = (double *) malloc(numColunas3 * sizeof(double)); 
		}

		for(int i = 0; i < numLinhas3; i++){
			for(int j = 0; j < numColunas3 - 1; j++){
				matrizResultado[i][j] = 0.0;
			}
		}

		//Realizando cálculo local
		int i, j, k, l;
		int inicio;
		int fim;
		inicio = procLocal * rank;
		fim = inicio + procLocal;
		l = 0;
		inicio = procLocal * rank;
		fim = inicio + procLocal;
		
		//#pragma omp parallel for shared(inicio, fim, numLinhas1, numColunas1, matrizResultado, matriz1, matriz2, i) private(j, k, l) schedule(static)
		for(i = inicio; i < fim; i++){
			for (j = 0; j < numLinhas1; j++){
				for (k = 0; k < numColunas1; k++){
					matrizResultado[l][j] = matrizResultado[l][j] + (matriz1[l][k] * matriz2[k][j]); 
				}
			}
			l++;
		}

		//Envia cálculo local
		double linha[numColunas3];
		for(int i = 0; i < procLocal; i++){
			for(int j = 0; j < numColunas3; j++){
				linha[j] = matrizResultado[i][j];
			}
			MPI_Send(linha, numColunas3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);			
		}
	}

	MPI_Finalize();
	return 0;
}