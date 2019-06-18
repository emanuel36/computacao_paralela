#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mpi.h"
#define SEED 35791246

int main(int argc, char** argv){
	int n;
	double x , y;
	int i, count = 0, countLocal = 0; 
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
		if(z <= 1) countLocal++;
	}

	MPI_Reduce(&countLocal, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){
		pi=(double)count/n*4;
   		printf("PI: %g\n", pi);
   	}

	MPI_Finalize();
}
