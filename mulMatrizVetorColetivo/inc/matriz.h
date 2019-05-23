#include <stdio.h>
#ifndef MATRIZ_H_ 
#define MATRIZ_H_

// Estrutura para guardar a matriz.
// Matriz Armazenada em ordem de linha: data[i][j] == data[i*m+j]
typedef struct Matriz { 
   int n;
   int m;
   float *data;
} Matriz;

// Função para carregar a matriz.
Matriz lerMatriz(FILE *arquivoMatriz);

// Função para imprimir a matriz.
void imprimirMatriz(Matriz matriz);

// Função para liberar a matriz.
void liberarMatriz(Matriz m);

#endif


