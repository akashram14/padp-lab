#include <iostream>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <vector>
using namespace std;

const int N = 1000000000;
bool is_prime[N + 1];
int cnt = 0;

void seive(){
	for (int i = 2; i <= N; i++){
		if (is_prime[i] && (long long) i * i <= N){
			for (int j = i * i; j <= N; j += i)
				is_prime[j] = false;
		}
		if(is_prime[i]) cnt++;
	}
}

void fastSeive(){
	int blocksize = 60000;	
	int nsqrt = sqrt(N);
	int *p = new int[blocksize];
	int starting_primes = 0;	
	bool *mark = new bool[blocksize + 1];	
	memset(mark, 1, blocksize + 1);	
	double start = omp_get_wtime();
	for(int i=2; i <= nsqrt; i++){
		if(mark[i]){
			p[starting_primes++] = i;
			for(int j=i*i; j <= nsqrt; j+=i)
				mark[j] = false;
		}
	}	
	bool *block = new bool[blocksize];
	for(int k=0; k*blocksize <= N; k++){
		 memset(block, 1, blocksize);
		 int start = k * blocksize;
		 for(int i=0; i<starting_primes; i++){
			 int start_idx = (start + p[i] - 1) / p[i];
			 int j = max(start_idx, p[i]) * p[i] - start;
			 for (; j < blocksize; j += p[i])
				 block[j] = 0;
		 }
		 if (k == 0)
            block[0] = block[1] = 0;
		 for(int i = 0; i < blocksize && start + i <= N; i++)
			 if(block[i])
				cnt++;
	 }
	 printf(" in %lf seconds\tNo.of Primes => %d\n", omp_get_wtime() - start, cnt);	
}


void fastestSeive(){
	int blocksize = sqrt(N);	
	int nsqrt = sqrt(N);
	int *p = new int[blocksize];
	int starting_primes = 0;
	bool *mark = new bool[blocksize + 1];
	memset(mark, 1, blocksize + 1);	
	double start = omp_get_wtime();
	#pragma omp parallel 
	{
		#pragma omp single
		{
			for(int i=2; i <= nsqrt; i++){
				if(mark[i]){
					p[starting_primes++] = i;
					for(int j=i*i; j <= nsqrt; j+=i)
						mark[j] = false;
				}
			}
		}	
		bool block[blocksize];
		#pragma omp for private(block) reduction(+:cnt)
		for(int k=0; k <= N/blocksize; k++){
			 memset(block, 1, blocksize);
			 int start = k * blocksize;
			 for(int i=0; i<starting_primes; i++){
				 int start_idx = (start + p[i] - 1) / p[i];
				 int j = max(start_idx, p[i]) * p[i] - start;
				 for (; j < blocksize; j += p[i])
					 block[j] = false;
			 }
			 if (k == 0)
				block[0] = block[1] = false;
			 for(int i = 0; i < blocksize && start + i <= N; i++)
				 if(block[i])
					cnt++;
		 }
	}
	 printf(" in %lf seconds\tNo.of Primes => %d\n", omp_get_wtime() - start, cnt);
}

int32_t main()
{
	double start;	
	cnt = 0;
	memset(is_prime, true, sizeof is_prime);
	printf("N => %d\n", N);
	start = omp_get_wtime();
	seive();
	printf(" in %lf seconds\tNo.of Primes => %d\n", omp_get_wtime() - start, cnt);
	cnt = 0;
	fastSeive();
	cnt = 0;
	fastestSeive();
	
}
