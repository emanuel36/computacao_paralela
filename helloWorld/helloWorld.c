#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
   int  numtasks, rank, len, rc; 
   char hostname[MPI_MAX_PROCESSOR_NAME];

   // Inicializar o Ambiente.  
   MPI_Init(&argc,&argv);

   // Recuperar o número de Processos.
   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

   // Recuperar o identificador do processo.
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);

   // Recuperar o nome da máquina.
   MPI_Get_processor_name(hostname, &len);
   printf ("Número de processos: %d Meu rank: %d Executando em %s\n", 
            numtasks, rank, hostname);

   // Realizar trabalho.... 

   // Finalizar o ambiente. 
   MPI_Finalize();
}