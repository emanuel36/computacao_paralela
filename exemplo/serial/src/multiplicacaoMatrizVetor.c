#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Matriz Armazenada em ordem de linha
// data[i][j] == data[i*m+j]
typedef struct Matriz { 
   int n;
   int m;
   float *data;
} Matriz;

typedef struct Vetor {
   int m;
   float *data;
} Vetor;

// Função auxiliar na leitura dos vetores/matrizes.
void adicionaFinalString(char *str, char c, int limite) {
   int tamanho = strlen(str);
   
   if (tamanho > limite)
      return;
  
   str[tamanho] = c;
   str[tamanho + 1] = '\0';

   return;
}

Matriz lerMatriz(FILE *arquivoMatriz){
   // Recupera as dimensões da Matriz
   char linhasString[10];
   char colunasString[10];
   fscanf(arquivoMatriz, "%[^\n]%*c", linhasString);
   int linhas = atoi(linhasString);
   fscanf(arquivoMatriz, "%[^\n]%*c", colunasString);
   int colunas = atoi(colunasString);
   printf("Lendo matriz com %d linhas e %d colunas\n", linhas, colunas);
   if (linhas == 0 || colunas == 0) {
      printf("Dimensões de matriz inválidas.\n");
      Matriz m;
      m.n = m.m = -1;
      m.data = NULL;
      return m;
   }

   // Aloca a matriz
   Matriz m;
   m.n = linhas;
   m.m = colunas;
   m.data = (float *) malloc(linhas * colunas * sizeof(float));
   
   // Fazer a leitura da Matriz
   int posicao = 0;
   char ch = fgetc(arquivoMatriz);
   char str[100] = "";
   while (ch != EOF && posicao < linhas * colunas) {
      if (ch == '\n' || ch == ':') {
         m.data[posicao] = atof(str);
         posicao++;
         strcpy(str, "");
      } else {
         adicionaFinalString(str, ch, 100);
      }
      ch = fgetc(arquivoMatriz);
   }
   
   return m;
}

Vetor lerVetor(FILE *arquivoVetor) {
   // Recupera o tamanho do vetor
   char tamanhoString[10];
   fscanf(arquivoVetor, "%[^\n]%*c", tamanhoString);
   int tamanho = atoi(tamanhoString);
   printf("Lendo vetor com %d elementos.\n", tamanho);
   if (tamanho == 0) {
      printf("Tamanho de vetor inválido.\n");
      Vetor v;
      v.m = -1;
      v.data = NULL;
      return v;
   }

   // Aloca o vetor
   Vetor v;
   v.m = tamanho;
   v.data = (float *) malloc(tamanho * sizeof(float));

   // Fazer a leitura do vetor
   int posicao = 0;
   char ch = fgetc(arquivoVetor);
   char str[100] = "";
   while (ch != EOF && posicao < tamanho) {
      if (ch == '\n' || ch == ':') {
         v.data[posicao] = atof(str);
	 posicao++;
	 strcpy(str, "");
      } else {
 	adicionaFinalString(str, ch, 100);
      }
      ch = fgetc(arquivoVetor);
   }
   return v;
}

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
   for (int i = 0; i < matriz.n; i++) {
      for (int j = 0; j < matriz.m; j++) 
         printf("%0.2f:", matriz.data[i * matriz.m + j]);
      printf("\n");
   }

   Vetor vetor = lerVetor(arquivoVetor);
   for (int i = 0; i < vetor.m; i++)
      printf("%0.2f:", vetor.data[i]);
   printf("\n");

   // Realiza a multiplicação.
   Vetor resultado;
   resultado.m = matriz.n;
   resultado.data = (float *) malloc(resultado.m * sizeof(float));
   for (int i = 0; i < resultado.m; i++) resultado.data[i] = 0.0;
   for (int i = 0; i < matriz.n; i++)
      for (int j = 0; j < matriz.m; j++)
         resultado.data[i] += matriz.data[i * matriz.m + j] * vetor.data[j];	      

   // Guarda o resultado no arquivo.
   FILE *arquivoResultado = fopen(nomeArquivoResultado, "w");
   fprintf(arquivoResultado, "%d\n", resultado.m);
   int i;
   for(i = 0; i < resultado.m - 1; i++)
      fprintf(arquivoResultado, "%0.2f:", resultado.data[i]);
   fprintf(arquivoResultado, "%0.2f", resultado.data[i]);
   fprintf(arquivoResultado, "\n");
 

   free(matriz.data);
   free(vetor.data);
   free(resultado.data);
   fclose(arquivoMatriz);
   fclose(arquivoVetor);
   fclose(arquivoResultado);

   return 0;
}
