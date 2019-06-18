#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mpi.h"
#define SEED 35791246

int main(int argc, char** argv){
	int n;
	double x , y;
	int i, count = 0; 
	double z;
	double pi;   
	srand(SEED);

	int numtasks, rank, len, rc; 
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int itr, rcv, countTotal = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Acões comuns à todos os processos
	n = atoi(argv[1]);
	itr = n/numtasks;

	for(i = 0; i < itr; i++){
		x = (double)rand()/RAND_MAX;
		y = (double)rand()/RAND_MAX;
		z = x*x+y*y;
		if(z <= 1) count++;
	}

	if(rank == 0){
		for(i = numtasks - 1; i > 0; i--){
			MPI_Recv(&rcv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			count += rcv; 
		}

		pi=(double)count/n*4;
   		printf("PI: %g\n", pi);

	}else{   
		MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
