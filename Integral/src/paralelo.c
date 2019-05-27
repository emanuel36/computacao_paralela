#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

double f(double x) {
	double return_val = 0.0;
	// return_val = x*x*x*x - x*x*x + x*x - x + 1;
	return_val = cos(x * log(1.0/x));
	return return_val;
}

int main(int argc, char *argv[]) {
	int numtasks, rank, len, rc; 
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Valor da integral
	double integralLocal = 0.0, integral = 0; 
	// Limites do intervalo
	double a, b;
	// Número de trapézios
	int n;
	// Base do trapézio
	double h;
	double x;
	int i;	

	a = atof(argv[1]);
	b = atof(argv[2]);
	n = atoi(argv[3]);

	int procLocal = n/numtasks;
	
	h = (b - a) / n;
	
	x = (((b - a) / numtasks) * rank) + a;

	for(i = 0; i < procLocal; i++){
		x += h;
		integralLocal += f(x);
	}

	MPI_Reduce(&integralLocal, &integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){
		integral += (f(a) + f(b))/2.0;	
		integral *= h;
		printf("%d trapézios, estimativa de %.2f a %.2f = %.5f\n", n, a, b, integral);
	}
	
	MPI_Finalize();
	return 0;
}