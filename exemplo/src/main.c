#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "matriz.h"

int main(int argc, char *argv[]) {
   // Variáveis do ambiente MPI	
   int  numeroDeProcessos, rank; 
   MPI_Status status;

   // Inicializa o ambiente MPI
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&numeroDeProcessos);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);

   // Variáveis das Matrizes
   Matriz matriz, vetor;
   int linhasLocais = 0;
 
   // Nomes dos arquivos. Usados apenas no rank == 0
   FILE *arquivoMatriz, *arquivoVetor;
   char *nomeArquivoResultado;

   // Leitura e distribuição da matriz e vetor.
   if (rank == 0) {
      // Nome do arquivo com a matriz.	
      char *nomeArquivoMatriz = argv[1];

      // Nome do arquivo com o vetor.
      char *nomeArquivoVetor = argv[2];

      // Nome do arquivo com o resultado.
      nomeArquivoResultado = argv[3];

      arquivoMatriz = fopen(nomeArquivoMatriz, "r");
      arquivoVetor = fopen(nomeArquivoVetor, "r");
      if ((arquivoMatriz == NULL) || (arquivoVetor == NULL)) {
         printf("Erro ao abrir os arquivos.\n");
         return 1;
      }

      matriz = lerMatriz(arquivoMatriz);
      // imprimirMatriz(matriz);

      vetor = lerMatriz(arquivoVetor);
      // imprimirMatriz(vetor);

      // Rank == 0 - Distribui o vetor B para todos os processos.
      for (int i = 1; i < numeroDeProcessos; i++) {
	 int dimensoesVetor[2] = {vetor.n, vetor.m};
         MPI_Send(dimensoesVetor, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
         MPI_Send(vetor.data, vetor.n * vetor.m, MPI_FLOAT, i, 1, MPI_COMM_WORLD);	 
      }

      // Rank == 0 - Distribui a partição da matriz para os processos.
      for (int i = 1; i < numeroDeProcessos; i++) {
	 int dimensoesMatriz[2] = {matriz.n, matriz.m};
         MPI_Send(dimensoesMatriz, 2, MPI_INT, i, 2, MPI_COMM_WORLD);
         linhasLocais = matriz.n / numeroDeProcessos;
         MPI_Send((i * linhasLocais * matriz.m) + matriz.data, linhasLocais * matriz.m, MPI_FLOAT, i, 3, MPI_COMM_WORLD);	 
      }

   } else {
      // Rank != 0 - Recebe o vetor B do rank 0.
      int dimensoesVetor[2];
      MPI_Recv(&dimensoesVetor, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      vetor.n = dimensoesVetor[0];
      vetor.m = dimensoesVetor[1];
      vetor.data = (float *) malloc(vetor.n * vetor.m * sizeof(float));
      MPI_Recv(vetor.data, vetor.n * vetor.m, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);

      // Rank != 0 - Recebe a partição da matriz do rank 0.
      int dimensoesMatriz[2];
      MPI_Recv(dimensoesMatriz, 2, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
      linhasLocais = dimensoesMatriz[0] / numeroDeProcessos;
      matriz.n = linhasLocais;
      matriz.m = dimensoesMatriz[1];
      matriz.data = (float *) malloc(matriz.n * matriz.m * sizeof(float));
      MPI_Recv(matriz.data, matriz.n * matriz.m, MPI_FLOAT, 0, 3, MPI_COMM_WORLD, &status);
   }   

   // Todos os ranks - Realiza o trabalho local.
   Matriz resultado;
   resultado.n = matriz.n;
   resultado.m = vetor.m;
   resultado.data = (float *) malloc(resultado.n * resultado.m * sizeof(float));
   
   for (int i = 0; i < resultado.n; i++) 
      for (int j = 0; j < resultado.m; j++)	   
         resultado.data[i * resultado.m + j] = 0.0;

   for (int i = 0; i < linhasLocais; i++)
      for (int j = 0; j < matriz.m; j++)
         resultado.data[i] += matriz.data[i * matriz.m + j] * vetor.data[j];	      

   // Consolidação dos Resultados
   if (rank == 0) {
      // Rank == 0 - Recupera de todos os processos o trabalho feito.
      for (int i = 1; i < numeroDeProcessos; i++) {
         MPI_Recv((i * resultado.m * linhasLocais) + resultado.data, linhasLocais * resultado.m, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status); 
      }

      // Rank == 0 - Guarda o resultado no arquivo.
      FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
      fprintf(arquivoResultado, "%d\n", resultado.n);
      fprintf(arquivoResultado, "%d\n", resultado.m);
      int i, j;
      for (i = 0; i < resultado.n; i++) {
         for(j = 0; j < resultado.m - 1; j++)
            fprintf(arquivoResultado, "%0.2f:", resultado.data[i * resultado.m + j]);
         fprintf(arquivoResultado, "%0.2f\n", resultado.data[i * resultado.m + j]);
      }
      fclose(arquivoMatriz);
      fclose(arquivoVetor);
      fclose(arquivoResultado);
   } else {
      // Rank != 0 - Envia o trabalho local para o rank 0.
      MPI_Send(resultado.data, resultado.n * resultado.m, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
   }
 
   free(matriz.data);
   free(vetor.data);
   free(resultado.data);
   MPI_Finalize();
   return 0;
}
