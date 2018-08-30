/* GRUPO 2 

- Leonardo de Souza Lemes (8941126)
- Tainá Andrello Piai (9266664)
- Rafael Amaro Rolfsen (9038648)
- Gabriel Toschi de Oliveira (9763039)
- Bruno Waldvogel (9842690)
- Rafael Bueno da Silva (8937271)
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define N 1000000000		// numero de iteracoes
#define T 64 // numero de threads

// estrutura que guarda os dados de argumento para cada thread
typedef struct {
	long long start;
	long long end;
} IterationData;

double sum = 0.0;
// mutex que regula o acesso à soma global
pthread_mutex_t globalSum_mutex;

// função que será executada pelas threads para calcular subsomas
void *piIteration(void *data){
	// recebe argumentos
	IterationData *itData = (IterationData *) data;

	double factor = 1.0;
	double calculation = 0.0;
	long long i;

	// calcula a subsoma para um intervalo
	for (i = itData->start; i <= itData->end; i++, factor = -factor){
		calculation += (factor / (2 * i + 1));
	}
	
	// tenta acessar a região crítica (soma global)
	pthread_mutex_lock(&globalSum_mutex);
		sum += calculation; // adiciona soma parcial à global
	pthread_mutex_unlock(&globalSum_mutex); // libera a região critica

	// desaloca, da memoria heap, a estrutura de argumentos
	free(itData);

	// termina a thread
	pthread_exit(0);
}

int main(void) {
	// inicializa vetor de thread IDs
	pthread_t* calculationThreads = (pthread_t *) malloc(sizeof(pthread_t)*N);

	// inicializa mutex que controla a soma global
	if (pthread_mutex_init(&globalSum_mutex, NULL)){
		// houve erro em inicializar o mutex
		printf("Erro em inicializar mutex.\n");
		exit(0);
	}

	double factor = 1.0;
	long long i;

	// calcula o numero de iteracoes que vai ser executada em cada thread
	int iterationsPerThread = N/T;
	IterationData* itData;
	
	// para cada thread que será feita
	for (i = 0; i < T; i++) {
		// monta a estrutura de argumentos
		itData = (IterationData *) malloc(sizeof(IterationData));
		itData->start = (iterationsPerThread * i);
		itData->end = (iterationsPerThread * i) + iterationsPerThread - 1;

		// inicializa a thread
		if (pthread_create(&(calculationThreads[i]), 0, (void *) piIteration, (void *) itData)){
			printf("Erro em inicializar thread %lli.\n", i);
			exit(0);
		}
	}

	// dá um join em todas as threads e espera sua conclusão
	for (i = 0; i < N; i++) pthread_join(calculationThreads[i], 0);

	// calcula a aproximacao de pi
	sum = 4 * sum;
	printf("pi = %.15f, N = %d  PARAL \n", sum, N);

	fflush(0);
	free(calculationThreads);
	return(0);
} // fim main()
