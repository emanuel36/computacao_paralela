#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>

int printArrayForRank(int rank, int *array,  int arraySize) {
	int i;
	char *output = (char *) malloc((arraySize + 100) * 4 * sizeof(char));
	sprintf(output, "Rank %d: ", rank);
	for (int i = 0; i < arraySize; i++)
		sprintf(output, "%s %d", output, array[i]);
	sprintf(output,"%s\n", output);
	printf("%s", output);
	fflush(stdout);
	free(output);
	return i;
}

int MPI_Ring_broadcast(int *array, int arraySize, MPI_Comm comm){
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int vizinho_esq = rank - 1;
	int vizinho_dir = rank + 1;

	if(rank == 0){
		vizinho_esq = size - 1;
		vizinho_dir = 1;
		MPI_Send(array, arraySize, MPI_INT, vizinho_esq, 0, comm);
		MPI_Send(array, arraySize, MPI_INT, vizinho_dir, 0, comm);
	}else if(rank >= (size/2)){
		if(rank == (size - 1)){
			MPI_Recv(array, arraySize, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);	
			MPI_Send(array, arraySize, MPI_INT, vizinho_esq, 0, comm);
		}else{
			MPI_Recv(array, arraySize, MPI_INT, vizinho_dir, 0, comm, MPI_STATUS_IGNORE);
			if(rank != (size/2)){
				MPI_Send(array, arraySize, MPI_INT, vizinho_esq, 0, comm);
			}
		}
	}else{
		MPI_Recv(array, arraySize, MPI_INT, vizinho_esq, 0, comm, MPI_STATUS_IGNORE);
		if(rank != ((size/2) - 1)){
			MPI_Send(array, arraySize, MPI_INT, vizinho_dir, 0, comm);
		}
	}
}

int MPI_Mesh_broadcast(int *array, int arraySize, MPI_Comm comm){
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int coords[2], dims[2] = {4,4}, periods[2] = {0,0}, reorder = 0, source1, dest1, source2, dest2;
	MPI_Comm comm_2d;

	MPI_Cart_create(comm, 2, dims, periods, reorder, &comm_2d);
	MPI_Cart_coords(comm_2d, rank, 2, coords);
	MPI_Cart_shift(comm_2d, 0, 1, &source1, &dest1);
	MPI_Cart_shift(comm_2d, 1, 1, &source2, &dest2);

	//se tiver na coluna 1...
	if(coords[1] == 0){
		//recebe de cima caso não seja a primeira linha
        if(rank != 0){
            MPI_Recv(array, arraySize, MPI_INT, source1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        //envia para o vizinho de baixo e para o vizinho da direita
        MPI_Send(array, arraySize, MPI_INT, dest2, 0, MPI_COMM_WORLD);
        MPI_Send(array, arraySize, MPI_INT, dest1, 0, MPI_COMM_WORLD);

    //se tiver em outra coluna (2, 3 ou 4)... 
    }else{
    	//recebe da esquerda e envia para a direita
        MPI_Recv(array, arraySize, MPI_INT, source2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(array, arraySize, MPI_INT, dest2, 0, MPI_COMM_WORLD);
    }
}


int main(int argc, char *argv[]) {
	int *array, arraySize, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	arraySize = atoi(argv[1]);
	array = (int *) malloc(arraySize * sizeof(int));

	// Inicializa o array com 0.
	if (rank == 0) {
		for (int i = 0; i < arraySize; i++)
			array[i] = 0;
	}

	// Imprime o array inicial de todos os processos.
	printArrayForRank(rank, array, arraySize);

	// Faz broadcast usando uma topologia anel.
	MPI_Ring_broadcast(array, arraySize, MPI_COMM_WORLD);

	// Barreira de sincronização.
	MPI_Barrier(MPI_COMM_WORLD);

	// Imprime o array após o broadcast na topologia anel.
	printArrayForRank(rank, array, arraySize);

	// Barreira de sincronização.
	MPI_Barrier(MPI_COMM_WORLD);

	// Reconfigura o array com todos os elementos iguais a 1000.
	if (rank == 0) {
		for (int i = 0; i < arraySize; i++)
			array[i] =  1000;
	}

	// Faz broadcast usando uma topologia mesh.
	MPI_Mesh_broadcast(array, arraySize, MPI_COMM_WORLD);

	// Barreira de sincronização.
	MPI_Barrier(MPI_COMM_WORLD);

	// Imprime o array após o broadcast na topologia mesh.
	printArrayForRank(rank, array, arraySize);

	free(array);
	MPI_Finalize();
	return 0;
}