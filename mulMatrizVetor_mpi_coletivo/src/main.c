#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matriz.h"
#include "mpi.h"

int main(int argc, char *argv[]) {
	int numtasks, rank, len, rc; 
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	float *vetor, *vetorResultado, *vetorMatriz, *vetorMatrizLocal;
	int numLinhas, numColunas, numElementos, numElementosLocal, tamanhoVetor, procLocal, tamanhoResultado;

	char *nomeArquivoResultado = argv[3];
	if(rank == 0){
		char *nomeArquivoMatriz = argv[1];
		char *nomeArquivoVetor = argv[2];	

		FILE *arquivoMatriz = fopen(nomeArquivoMatriz, "r");
		FILE *arquivoVetor = fopen(nomeArquivoVetor, "r");
		if ((arquivoMatriz == NULL) || (arquivoVetor == NULL)) {
			printf("Erro ao abrir os arquivos.\n");
			return 1;
		}

		Matriz matrizAux = lerMatriz(arquivoMatriz);
		Matriz vetorAux = lerMatriz(arquivoVetor);

		//salvando matriz e vetor na memória
		tamanhoVetor = vetorAux.n;
		vetor = vetorAux.data;
		numLinhas = matrizAux.n;
		numColunas = matrizAux.m;
		float matriz[numLinhas][numColunas];

		for(int i = 0; i < numLinhas; i++){
			for(int j = 0; j < numColunas; j++){
				matriz[i][j] = matrizAux.data[i * matrizAux.m + j];
			}
		}

		//Calculando quantidade de trabalho local
		procLocal = matrizAux.n/numtasks;

		//Transformando matriz em vetor
		numElementos = numLinhas * numColunas;
		vetorMatriz = (float *) malloc(numElementos * sizeof(float));
		int z = 0;
		for(int i = 0; i < numLinhas; i++){
			for(int j = 0; j < numColunas; j++){
				vetorMatriz[z++] = matriz[i][j];
			}
		}
	}
	//enviando o tamanho do vetor para os processos
	MPI_Bcast(&tamanhoVetor, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//enviando o vetor para os processos
	if(rank != 0){
		vetor = (float *) malloc(tamanhoVetor * sizeof(float));
	}
	MPI_Bcast(vetor, tamanhoVetor, MPI_FLOAT, 0, MPI_COMM_WORLD);

	//Enviando processamento local para os processos
	MPI_Bcast(&procLocal, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//separando partes da matriz entre os processos
	numColunas = tamanhoVetor;
	numElementosLocal = numColunas * procLocal;
	vetorMatrizLocal = (float *) malloc(numElementosLocal * sizeof(float));
	MPI_Scatter(vetorMatriz, numElementosLocal, MPI_FLOAT, vetorMatrizLocal, numElementosLocal, MPI_FLOAT, 0, MPI_COMM_WORLD);

	
	//Converte array em matriz
	int z = 0;
	float matrizLocal[procLocal][numColunas];
	for(int i = 0; i < procLocal; i++){
		for(int j = 0; j < numColunas; j++){
			matrizLocal[i][j] = vetorMatrizLocal[z++];
		}
	}

	//cálculo local
	int tamanhoResultadoLocal = procLocal;
	float vetorResultadoLocal[tamanhoResultadoLocal];
	for(int i = 0; i < tamanhoResultadoLocal; i++){
		vetorResultadoLocal[i] = 0.0;
	}
	
	for(int i = 0; i < procLocal; i++){
		for(int j = 0; j < numColunas; j++){
			vetorResultadoLocal[i] += vetor[j] * matrizLocal[i][j];
		}
	}

	//juntando as contribuições de cada processo
	tamanhoResultado = numtasks * procLocal;
	vetorResultado = (float *) malloc(tamanhoResultado * sizeof(float));
	MPI_Gather(vetorResultadoLocal, procLocal, MPI_FLOAT, vetorResultado, procLocal, MPI_FLOAT, 0, MPI_COMM_WORLD);

	
	if(rank == 0){
		//Armazena o resultado no arquivo.
		FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
		fprintf(arquivoResultado, "%d\n", tamanhoResultado);
		fprintf(arquivoResultado, "1\n");
		for (int i = 0; i < tamanhoResultado; i++) {
			fprintf(arquivoResultado, "%0.2f\n", vetorResultado[i]);
		}

		// free(matrizAux.data);
		// free(vetorAux.data);
		// fclose(arquivoMatriz);
		// fclose(arquivoVetor);
		// fclose(arquivoResultado);
	}

	MPI_Finalize();
	return 0;
}