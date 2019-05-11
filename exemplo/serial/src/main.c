#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matriz.h"

int main(int argc, char *argv[]) {
   // Nome do arquivo com a matriz.	
   char *nomeArquivoMatriz = argv[1];

   // Nome do arquivo com o vetor.
   char *nomeArquivoVetor = argv[2];

   // Nome do arquivo com o resultado.
   char *nomeArquivoResultado = argv[3];

   FILE *arquivoMatriz = fopen(nomeArquivoMatriz, "r");
   FILE *arquivoVetor = fopen(nomeArquivoVetor, "r");
   if ((arquivoMatriz == NULL) || (arquivoVetor == NULL)) {
      printf("Erro ao abrir os arquivos.\n");
      return 1;
   }

   Matriz matriz = lerMatriz(arquivoMatriz);
   imprimirMatriz(matriz);

   Matriz vetor = lerMatriz(arquivoVetor);
   imprimirMatriz(vetor);
   
   // Realiza a multiplicação.
   Matriz resultado;
   resultado.n = matriz.n;
   resultado.m = vetor.m;
   resultado.data = (float *) malloc(resultado.n * sizeof(float));
   for (int i = 0; i < resultado.n; i++) resultado.data[i] = 0.0;
   for (int i = 0; i < matriz.n; i++)
      for (int j = 0; j < matriz.m; j++)
         resultado.data[i] += matriz.data[i * matriz.m + j] * vetor.data[j];	      

   // Guarda o resultado no arquivo.
   FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
   fprintf(arquivoResultado, "%d\n", resultado.n);
   fprintf(arquivoResultado, "%d\n", resultado.m);
   int i, j;
   for (i = 0; i < resultado.n; i++) {
      for(j = 0; j < resultado.m - 1; j++)
         fprintf(arquivoResultado, "%0.2f:", resultado.data[i * resultado.m + j]);
      fprintf(arquivoResultado, "%0.2f\n", resultado.data[i * resultado.m + j]);
   }
 
   free(matriz.data);
   free(vetor.data);
   free(resultado.data);
   fclose(arquivoMatriz);
   fclose(arquivoVetor);
   fclose(arquivoResultado);

   return 0;
}
