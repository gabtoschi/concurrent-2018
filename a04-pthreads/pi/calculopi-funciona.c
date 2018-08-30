// Calcula uma aproximacao do numero pi (3,14159 26535 89793 23846 26433 83279)
// algoritmo baseado em Pacheco (2011), pp 163, Seção 4.4 e Program 4.3.
//
// para compilar: gcc calculo_pi_pacheco_seq.c -o calculo_pi_pacheco_seq
// para executar: calculo_pi_pacheco_seq
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define N 1000000000		// numero de iteracoes
#define T 64 // numero de threads

typedef struct {
	long long start;
	long long end;
} IterationData;

double sum = 0.0;
pthread_mutex_t globalSum_mutex;

void *piIteration(void *data){
	IterationData *itData = (IterationData *) data;

	double factor = 1.0;
	double calculation = 0.0;
	long long i;

	for (i = itData->start; i <= itData->end; i++, factor = -factor){
		calculation += (factor / (2 * i + 1));
	}
	
	pthread_mutex_lock(&globalSum_mutex);
		printf("S = %lli | E = %lli | calculated %lf\n", itData->start, itData->end, calculation);
		sum += calculation;
	pthread_mutex_unlock(&globalSum_mutex);

	free(itData);

	pthread_exit(0);
}

int main(void) {
	// initialize thread array
	pthread_t* calculationThreads = (pthread_t *) malloc(sizeof(pthread_t)*N);

	// initialize the global sum mutex
	pthread_mutex_init(&globalSum_mutex, NULL);

	double factor = 1.0;
	long long i;
	int iterationsPerThread = N/T;
	IterationData* itData;
	
	for (i = 0; i < T; i++) {
		itData = (IterationData *) malloc(sizeof(IterationData));
		itData->start = (iterationsPerThread * i);
		itData->end = (iterationsPerThread * i) + iterationsPerThread - 1;

		pthread_create(&(calculationThreads[i]), 0, (void *) piIteration, (void *) itData);
	}

	for (i = 0; i < N; i++) pthread_join(calculationThreads[i], 0);

	sum = 4 * sum;
	printf("pi = %.15f, N = %d  PARAL \n", sum, N);

	fflush(0);
	free(calculationThreads);
	return(0);
} // fim main()
