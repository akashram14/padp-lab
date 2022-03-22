#include <iostream>
#include <omp.h>

const int N = 1300;

int32_t main(){
	
	double *A, *B, *C;
	int Ndim, Pdim, Mdim;
	int i, j, k;
	
	Ndim = N;
	Pdim = N;
	Mdim = N;
	
	A = (double *)malloc(Ndim*Pdim*sizeof(double));
  B = (double *)malloc(Pdim*Mdim*sizeof(double));
  C = (double *)malloc(Ndim*Mdim*sizeof(double));
	
	
	for (i=0; i<Ndim; i++)
		for (j=0; j<Pdim; j++)
			*(A+(i*Ndim+j)) = rand();
			
	for (i=0; i<Pdim; i++)
		for (j=0; j<Mdim; j++)
			*(B+(i*Pdim+j)) = rand();

	for (i=0; i<Ndim; i++)
		for (j=0; j<Mdim; j++)
			*(C+(i*Ndim+j)) = 0.0;
			
			
	int threads;
	for(threads=1; threads <= 8; threads = threads<<1){
		double start = omp_get_wtime();
		int i,j,k;
		#pragma omp parallel for private(j,k) num_threads(threads)
		for(i=0; i<Ndim; i++){
			for(j=0; j<Mdim; j++){
				for(k=0; k<Pdim; k++)
					*(C+(i*Ndim+j)) += *(A+(i*Ndim+k)) *  *(B+(k*Pdim+j));
			}
		}
		double end = omp_get_wtime();
		printf("Threads => %d and time => %f\n", threads, (end - start));
	}
	
}
