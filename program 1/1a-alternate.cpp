#include <iostream>
#include <math.h>
#include <omp.h>
using namespace std;
const double PI = 2*acos(0.0);
static long num_trials = 10000;

int32_t main(){ 	
	   long i;  long Ncirc;
	   double pi, x, y, test, start;
	   unsigned int seed;
	   for(int l=1; l<=8; l<<=1){
		   Ncirc = 0;
		   printf(" %d threads ",l);
		   srand(time(NULL));
		   start = omp_get_wtime();
			  
		  #pragma omp parallel private(x,y,test,seed) num_threads(l)
		  {
			  seed = time(NULL) + omp_get_thread_num();
			  #pragma omp for reduction(+:Ncirc)
			  for(i=0;i<num_trials; i++)
			  {
				 x = (double)rand_r(&seed)/RAND_MAX; 
				 y = (double)rand_r(&seed)/RAND_MAX;

				 test = x*x + y*y;

				 if (test <= 1) Ncirc++;
			   }
		   }

			pi = 4.0 * ((double)Ncirc/(double)num_trials);

			printf("\n %ld trials, pi is %lf ",num_trials, pi);
			printf(" in %lf seconds\tError => %f\n",omp_get_wtime()-start, fabs(PI-pi));
		}
}
