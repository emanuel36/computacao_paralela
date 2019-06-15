#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matriz.h"

int main(int argc, char *argv[]) {
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
	float matriz1[numLinhas1][numColunas1];
	numLinhas2 = matriz2Aux.n;
	numColunas2 = matriz2Aux.m;
	float matriz2[numLinhas2][numColunas2];

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

	int numLinhas3 = numColunas1;
	int numColunas3 = numLinhas2;

	float matrizResultado[numLinhas3][numColunas3];

	for(int i = 0; i < numLinhas3; i++){
		for(int j = 0; j < numColunas3 - 1; j++){
			matrizResultado[i][j] = 0.0;
		}
	}

	//Realizando cálculo
	for (int i = 0; i < numLinhas1; i++){
		for (int j = 0; j < numLinhas1; j++){
			for (int k = 0; k < numColunas1; k++){
				matrizResultado[i][j] = matrizResultado[i][j] + (matriz1[i][k] * matriz2[k][j]); 
			}
		}
	}
	
	//Armazena o resultado no arquivo.
	FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
	fprintf(arquivoResultado, "%d\n", numLinhas3);
	fprintf(arquivoResultado, "%d\n", numColunas3);
	for(int i = 0; i < numLinhas3; i ++){
		for(int j = 0; j < (numColunas3 - 1); j++){
			fprintf(arquivoResultado, "%.1f", matrizResultado[i][j]);
			if(j < (numColunas3 - 2)){
				fprintf(arquivoResultado, ":");
			}
		}
		fprintf(arquivoResultado, "\n");
	}

	return 0;
}