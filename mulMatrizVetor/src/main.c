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

	if(rank == 0){
		char *nomeArquivoMatriz = argv[1];
		char *nomeArquivoVetor = argv[2];
		char *nomeArquivoResultado = argv[3];

		FILE *arquivoMatriz = fopen(nomeArquivoMatriz, "r");
		FILE *arquivoVetor = fopen(nomeArquivoVetor, "r");
		if ((arquivoMatriz == NULL) || (arquivoVetor == NULL)) {
			printf("Erro ao abrir os arquivos.\n");
			return 1;
		}

		Matriz matrizAux = lerMatriz(arquivoMatriz);
		Matriz vetorAux = lerMatriz(arquivoVetor);

		//salvando matriz e vetor na memória
		float *vetor = vetorAux.data;
		int numLinhas, numColunas;
		numLinhas = matrizAux.n;
		numColunas = matrizAux.m;
		float matriz[numLinhas][numColunas];

		for(int i = 0; i < numLinhas; i++){
			for(int j = 0; j < numColunas; j++){
				matriz[i][j] = matrizAux.data[i * matrizAux.m + j];
			}
		}

		//Enviando tamanho do vetor para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&vetorAux.n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando o vetor para os processos
		for(int i = 0; i < numtasks; i++){
			MPI_Send(vetorAux.data, vetorAux.n, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}

		//Calculando quantidade de trabalho local
		int procLocal = matrizAux.n/numtasks;

		//Enviando processamento local para os processos
		for(int i = 1; i < numtasks; i++){
			MPI_Send(&procLocal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//Enviando partes da matriz para os processos	
		float linha[numColunas];
		int inicioV, fimV;

		for(int i = 1; i < numtasks; i++){
			inicioV = procLocal * i; 
			fimV = inicioV + procLocal;
			for(int j = inicioV; j < fimV; j++){
				for(int z = 0; z < numColunas; z++){
					linha[z] = matriz[j][z];
				}
				MPI_Send(linha, numColunas, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			}
		}

		//Realiza Cálculo local
		int tamanhoResultado = numLinhas;
		float vetorResultado[tamanhoResultado];

		for(int i = 0; i < tamanhoResultado; i++)	vetorResultado[i] = 0;

		for(int i = 0; i < procLocal; i++){
			for(int j = 0; j < numColunas; j++){
				vetorResultado[i] += vetor[j] * matriz[i][j];
			}
		}

		//recebendo contribuições dos processos
		float vetorTmp[procLocal];
		for(int i = 1; i < numtasks; i++){
			MPI_Recv(vetorTmp, procLocal, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < procLocal; j++){
				vetorResultado[(i * procLocal) + j] = vetorTmp[j];
			}
		}

		//Armazena o resultado no arquivo.
		FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
		fprintf(arquivoResultado, "%d\n", tamanhoResultado);
		fprintf(arquivoResultado, "1\n");
		for (int i = 0; i < tamanhoResultado; i++) {
			fprintf(arquivoResultado, "%0.2f\n", vetorResultado[i]);
		}

		//IMPRIMIR MATRIZ
		// printf("\n");
		// for(int i = 0; i < numLinhas; i++){
		// 	for(int j = 0; j < numColunas - 1; j++){
		// 		printf("%.1f ", matriz[i][j]);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");

		//IMPRIMIR VETOR FINAL
		// for(int i = 0; i < tamanhoResultado; i++){
		// 	printf("RANK %d - Vetor [%d]: %.1f\n", rank, i, vetorResultado[i]);
		// }

		free(matrizAux.data);
		free(vetorAux.data);
		fclose(arquivoMatriz);
		fclose(arquivoVetor);
		fclose(arquivoResultado);
	}else{
		//Recebendo tamanho do vetor
		int tamanhoVetor;
		MPI_Recv(&tamanhoVetor, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Recebendo vetores
		float *vetor = (float *) malloc(tamanhoVetor * sizeof(float)); 
		MPI_Recv(vetor, tamanhoVetor, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Recebendo processamento local
		int procLocal;
		MPI_Recv(&procLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Recebendo partes da matriz
		int numColunas = tamanhoVetor;
		float matriz[procLocal][numColunas];
		float vetorAux[numColunas];
		for(int i = 0; i < procLocal; i++){
			MPI_Recv(&vetorAux, numColunas, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < numColunas; j++){
				matriz[i][j] = vetorAux[j];
			}
		}

		//Realiza cálculo local
		int tamanhoResultadoLocal = procLocal;
		float vetorResultadoLocal[tamanhoResultadoLocal];

		for(int i = 0; i < procLocal; i++){
			for(int j = 0; j < numColunas; j++){
				vetorResultadoLocal[i] += vetor[j] * matriz[i][j];
			}
		}

		//Envia cálculo local
		MPI_Send(vetorResultadoLocal, procLocal, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

	}

	MPI_Finalize();
	return 0;
}