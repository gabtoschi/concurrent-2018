/* 
--- GRUPO 2 ---
Vitor Trevelin Xavier da Silva - 9791285
Murilo Bernard Theodoro - 8532130
Gabriel Toschi de Oliveira - 9763039
*/

#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>

#define NOSPAWN 3

// Based on Author: Ana Caroline Spengler

// função basica para calcular maximo entre dois numeros
int max (int a, int b){
  if (a > b) return a;
  else return b;
}

// função basica para calcular minimo entre dois numeros
int min (int a, int b){
  if (a < b) return a;
  else return b;
}

// para dar semântica aos índices do vetor de buffer entre os processos
typedef enum vetorBuffer {
  DIFERENCA,
  MENORI,
  MENORJ,
  MAIORI,
  MAIORJ,
  VALORMAIOR,
  VALORMENOR,
  TAMBUFFER
};

int main(void){

    // Inicializando MPI
    MPI_Init(NULL, NULL);

    // Coletando Rank e numero de processos gerados pelo MPI
    int noProcessos, meuRank;
    MPI_Comm_size(MPI_COMM_WORLD, &noProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &meuRank);

    // intercomunicador do spawn
    MPI_Comm intercomm;
    MPI_Comm_get_parent(&intercomm);

    //printf("fui spawnado, sou o rank %d\n", meuRank);

    // recebendo ordem da matriz por broadcast
    int tam;
    MPI_Bcast(&tam, 1, MPI_INT, 0, intercomm);

    // receber quantidade de elementos a alocar
    int quantAlocacao;
    MPI_Recv(&quantAlocacao, 1, MPI_INT, 0, 0, intercomm, NULL);

    int *matriz = (int *) malloc(quantAlocacao * sizeof(int));

    // receber matriz
    MPI_Scatterv(NULL, NULL, NULL, NULL, matriz, quantAlocacao, MPI_INT, 0, intercomm);

    int i;
    printf("rank %d: ", meuRank);
    for (i = 0; i < quantAlocacao; i++) printf("%d ", matriz[i]);
    printf("\n");

    //printf("rank %d aloca %d\n", meuRank, quantAlocacao);

    //printf("rank %d, ordem %d\n", meuRank, tam);

    free(matriz);
    // Fechando MPI
    MPI_Finalize();

  return 0;
}
