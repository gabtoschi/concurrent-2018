/* Ali Husseinat - 9292966 */

#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>

// Based on Author: Ana Caroline Spengler

int max (int a, int b){
  if (a > b) return a;
  else return b;
}

int min (int a, int b){
  if (a < b) return a;
  else return b;
}

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

    int noProcessos, meuRank;
    MPI_Comm_size(MPI_COMM_WORLD, &noProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &meuRank);

    //Criação e abertura dos arquivos de entrada e saída
    FILE *arquivo_entrada/*,*arquivo_saida*/;
    arquivo_entrada=fopen("numeros.txt","r");
    //arquivo_saida=fopen("saida.txt","w+");

    //Tamanho da matriz
    int tam;
    //Variáveis auxiliares
    int i, j, n, m;

    //Leitura do tamanho da matriz
    fscanf(arquivo_entrada, "%d\n", &tam);

    int tamanhoBloco = tam / noProcessos;
    int restoBloco = tam % noProcessos;
    int minRange = 0, maxRange = 0;

    if (meuRank < restoBloco) tamanhoBloco++;
    else {
      minRange += (restoBloco * (tamanhoBloco + 1));
      maxRange += (restoBloco * (tamanhoBloco + 1));
    }

    minRange += (tamanhoBloco * meuRank);
    maxRange += (tamanhoBloco * meuRank) + (tamanhoBloco - 1);

    int minLeitura = max(minRange - 1, 0);
    int maxLeitura = min(maxRange + 1, tam - 1);

    //Matriz
    int **matriz=((int**)malloc((maxLeitura - minLeitura + 1)*sizeof(int*)));

    printf("Rank %d | MinLeitura %d | MaxLeitura %d\n", meuRank, minLeitura, maxLeitura);

    //Alocação da matriz
    for(i = 0; i < (maxLeitura - minLeitura + 1); i++)
        matriz[i]=((int*)malloc(tam*sizeof(int)));

    //Leitura da matriz
    for (i = 0; i < minLeitura * tam; i++) fscanf(arquivo_entrada, "%d\n", &j);

    for(i = 0; i < (maxLeitura - minLeitura + 1); i++){
        for(j = 0; j < tam; j++)
            fscanf(arquivo_entrada, "%d\n", &(matriz[i][j]));
    }

  //Auxiliar para comparar o valor da diferenca entre os vizinhos
  int aux_comp;
  //Variável para guardar a maior diferenca encontrada
  int diferenca = INT_MIN;
  //Variáveis para armazenar os indices da maior diferença encontrada
  int indice_i_maior, indice_j_maior, indice_i_menor, indice_j_menor;
  //Variáveis para armazenar os valores da maior diferença encontrada
  //Poderia utilizar apenas os indices armazenados, colocados aqui por questões de clareza
  int valor_maior, valor_menor;

  int start = (meuRank == 0) ? 0 : 1;
  int end = (meuRank == noProcessos - 1) ? tamanhoBloco - 1 : tamanhoBloco - 2;

  for (i = start; i <= end; i++) {
    for (j = 0; j < tam; j++) {

      //Avalia a diferenca entre o elemento matriz[i][j] e seus vizinhos
      for (n = i - 1; n < i + 2; n++) {
        for (m = j - 1; m < j + 2; m++) {
          //Se não for a borda da matriz, tire a diferenca
          if (!(n < 0 || n > i || m < 0 || j > m)){
            //Módulo da diferenca entre o elemento matriz[i][j] e seu vizinho
            aux_comp = abs(matriz[i][j] - matriz[n][m]);
            //Se a comparação atual é maior que a armazenada
            if (aux_comp > diferenca){
              //Diferença global é atualizada
              diferenca = aux_comp;
              //Armazenando as posições da maior diferença encontrada até o momento
              indice_i_maior = i;
              indice_j_maior = j;
              indice_i_menor = n;
              indice_j_menor = m;
              //Armazenando o valor da maior diferença encontrada até o momento
              valor_maior = matriz[i][j];
              valor_menor = matriz[n][m];
            }
          }
        }
      }
    }
  }

  // Corrigir os indices das matrizes
  indice_i_maior += minLeitura;
  indice_i_menor += minLeitura;

  //Escrita dos resultados no arquivo de saída no padrão solicitado pelo exemplo
  printf("M[%d,%d]=%d M[%d,%d]=%d" ,indice_i_maior, indice_j_maior, valor_maior, indice_i_menor, indice_j_menor, valor_menor);


  /*// criar vetor de buffer
  int *buffer;
  MPI_Status status;

  if (meuRank != 0){
    buffer = (int *) malloc(sizeof(int)*TAMBUFFER);
    buffer[DIFERENCA] = diferenca;
    buffer[MENORI] = indice_i_menor;
    buffer[MAIORI] = indice_i_maior;
    buffer[MENORJ] = indice_j_menor;
    buffer[MAIORJ] = indice_j_maior;
    buffer[VALORMENOR] = valor_menor;
    buffer[VALORMAIOR] = valor_maior;

    MPI_Send(buffer, TAMBUFFER, MPI_INT, 0, meuRank, MPI_COMM_WORLD);
  } else {
    for (i = 1; i < noProcessos; i++){
      MPI_Recv(buffer, TAMBUFFER, MPI_INT, i, i, MPI_COMM_WORLD, &status);

      if (buffer[DIFERENCA] > diferenca){
        diferenca = buffer[DIFERENCA];
        indice_i_menor = buffer[MENORI];
        indice_i_maior = buffer[MAIORI];
        indice_j_menor = buffer[MENORJ];
        indice_j_maior = buffer[MAIORJ];
        valor_menor = buffer[VALORMENOR];
        valor_maior = buffer[VALORMAIOR];
      }

      //free(buffer);
      buffer = NULL;
    }
  }

  //Escrita dos resultados no arquivo de saída no padrão solicitado pelo exemplo
  printf("M[%d,%d]=%d M[%d,%d]=%d" ,indice_i_maior, indice_j_maior, valor_maior, indice_i_menor, indice_j_menor, valor_menor);
*/
  //Liberar a matriz
  for(i = 0; i < (maxLeitura - minLeitura + 1); i++)
    free(matriz[i]);

  free(matriz);

  //Fechando os arquivos de entrada e saída
  fclose(arquivo_entrada);
  //fclose(arquivo_saida);

  // Fechando MPI
  MPI_Finalize();

  return 0;
}
