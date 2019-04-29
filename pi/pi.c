#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define SEED 35791246

int main(int argc, char** argv){
   int n = atoi(argv[1]);
   double x,y;
   int i,count=0; /* # of points in the 1st quadrant of unit circle */
   double z;
   double pi;   

   /* initialize random numbers */
   srand(SEED);
   count=0;
   for ( i=0; i<n; i++) {
      x = (double)rand()/RAND_MAX;
      y = (double)rand()/RAND_MAX;
      z = x*x+y*y;
      if (z<=1) count++;
      }
   pi=(double)count/n*4;
   printf("PI: %g\n", pi);
}
