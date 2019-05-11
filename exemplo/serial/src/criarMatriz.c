#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
   // Recuperar as dimensões da matriz.
   int linhas = atoi(argv[1]);
   int colunas = atoi(argv[2]);

   // Recuperar nome do arquivo para salvar a matriz.
   char *nomeDoArquivo = argv[3];

   // Abrir arquivo para escrita.
   FILE *arquivo = fopen(nomeDoArquivo, "w+");

   // Faixa de valores aleatórios
   srand((unsigned int)time(NULL));
   float faixaAleatoria = 100.00;

   // Escreve a ordem.
   fprintf(arquivo, "%d\n", linhas);
   fprintf(arquivo, "%d\n", colunas);
   for (int i = 0; i < linhas; i++) {
      for (int j = 0; j < colunas; j++) {
         float valorAleatorio = ((float)rand()/(float)(RAND_MAX)) * faixaAleatoria;
         if (j < colunas - 1)
            fprintf(arquivo, "%05.2f:", valorAleatorio);
	 else 
            fprintf(arquivo, "%05.2f", valorAleatorio);
      }
      fprintf(arquivo, "\n");
   }

   // Escreve os elementos.

   // Fecha arquivo.
   fclose(arquivo);

   return 0;
}
