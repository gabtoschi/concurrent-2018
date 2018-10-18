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

    printf("master tem %d processos\n", noProcessos);

    //Criação e abertura dos arquivos de entrada e saída
    FILE *arquivo_entrada/*,*arquivo_saida*/;
    arquivo_entrada=fopen("numeros.txt","r");
    //arquivo_saida=fopen("saida.txt","w+");

    //Tamanho da matriz
    int tam;
    //Variáveis auxiliares
    int i, j, n, m;

    // intercomunicador dos processos spawnados
    MPI_Comm intercomm;

    // Vetor de codigos de erro
    int errcodes[10];

    // spawn dos processos slave
    MPI_Comm_spawn("exslave", MPI_ARGV_NULL, NOSPAWN, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes);

    //Leitura do tamanho da matriz
    fscanf(arquivo_entrada, "%d\n", &tam);

    // broadcast (master -> slaves) da ordem da matriz
    MPI_Bcast(&tam, 1, MPI_INT, MPI_ROOT, intercomm);

    int *matriz = (int *) malloc(tam * tam * sizeof(int));

    for(i = 0; i < tam; i++){
        for(j = 0; j < tam; j++)
            fscanf(arquivo_entrada, "%d\n", &(matriz[i*tam+j]));
    }

    /*for(i = 0; i < tam; i++){
        for(j = 0; j < tam; j++)
            printf("%d ", matriz[i*tam+j]);
        printf("\n");
    }*/

    int noDivisoes = NOSPAWN + 1;

    // calculando quantas linhas da matriz cada processo calculara
    int tamanhoBloco = tam / noDivisoes;
    int restoBloco = tam % noDivisoes;

    int *quantidadeElem = (int *) malloc(NOSPAWN * sizeof(int));
    int *offsetsMatriz = (int *) malloc(NOSPAWN * sizeof(int));

    int tamanhoSpawn, minRange, maxRange, minLeitura, maxLeitura, quantAlocacao;
 
    for (i = 0; i <= NOSPAWN; i++){
      tamanhoSpawn = tamanhoBloco;
      minRange = maxRange = minLeitura = maxLeitura = 0;

      if (i < restoBloco) {
        tamanhoSpawn++;

        minRange += (tamanhoSpawn * i);
        maxRange += (tamanhoSpawn * i) + (tamanhoSpawn - 1);
      } else {
        minRange += (restoBloco * (tamanhoSpawn + 1));
        maxRange += (restoBloco * (tamanhoSpawn + 1));

        minRange += (tamanhoSpawn * (i - restoBloco));
        maxRange += (tamanhoSpawn * (i - restoBloco)) + (tamanhoSpawn - 1);
      }

      minLeitura = max(minRange - 1, 0);
      maxLeitura = min(maxRange + 1, tam - 1);

      if (i != NOSPAWN) {
        quantidadeElem[i] = (maxLeitura - minLeitura + 1) * tam;
        offsetsMatriz[i] = tam * minLeitura;

        MPI_Ssend(&quantidadeElem[i], 1, MPI_INT, i, 0, intercomm);
      }
    }

    MPI_Scatterv(matriz, quantidadeElem, offsetsMatriz, MPI_INT, NULL, NULL, NULL, MPI_ROOT, intercomm);
/*
    

    /

    // calculando com quais linhas esse processo específico ficará
    if (meuRank < restoBloco) tamanhoBloco++;
    else {
      minRange += (restoBloco * (tamanhoBloco + 1));
      maxRange += (restoBloco * (tamanhoBloco + 1));
    }

    minRange += (tamanhoBloco * meuRank);
    maxRange += (tamanhoBloco * meuRank) + (tamanhoBloco - 1);

    printf("rank %d, minrange %d, maxrange %d\n", meuRank, minRange, maxRange);

    // calculando quais linhas o processo lerá do arquivo 
    // (ele também lerá as vizinhas para diminuir comunicações entre processos)
    int minLeitura = max(minRange - 1, 0);
    int maxLeitura = min(maxRange + 1, tam - 1);

    printf("rank %d, minleitura %d, maxleitura %d\n", meuRank, minLeitura, maxLeitura);


    //Matriz
    int *matriz=((int*)malloc((maxLeitura - minLeitura + 1)*tam*sizeof(int)));

    //Alocação da matriz
    //for(i = 0; i < (maxLeitura - minLeitura + 1); i++)
    //    matriz[i]=((int*)malloc(tam*sizeof(int)));

    //Leitura da matriz
    for (i = 0; i < minLeitura * tam; i++) fscanf(arquivo_entrada, "%d\n", &j);

    

  //Auxiliar para comparar o valor da diferenca entre os vizinhos
  int aux_comp;
  //Variável para guardar a maior diferenca encontrada
  int diferenca = INT_MIN;
  //Variáveis para armazenar os indices da maior diferença encontrada
  int indice_i_maior, indice_j_maior, indice_i_menor, indice_j_menor;
  //Variáveis para armazenar os valores da maior diferença encontrada
  //Poderia utilizar apenas os indices armazenados, colocados aqui por questões de clareza
  int valor_maior, valor_menor;

  // calculando indices de linha que serao percorridos no bloco da matriz
  int start = (meuRank == 0) ? 0 : 1;
  int end = (meuRank == noProcessos - 1) ? tamanhoBloco : tamanhoBloco - 1;

  printf("myrank %d, start %d, end %d\n", meuRank, start, end);

  for (i = start; i <= end; i++) {
    for (j = 0; j < tam; j++) {

      //Avalia a diferenca entre o elemento matriz[i][j] e seus vizinhos
      for (n = i - 1; n < i + 2; n++) {
        for (m = j - 1; m < j + 2; m++) {
          //Se não for a borda da matriz, tire a diferenca
          if (!(n < 0 || n > i || m < 0 || j < m)){
            //Módulo da diferenca entre o elemento matriz[i][j] e seu vizinho
            aux_comp = abs(matriz[i*tam+j] - matriz[n*tam+m]);
            //printf("dif: %d, i %d, j %d, n %d, m %d\n", aux_comp, i, j, n, m);
            //Se a comparação atual é maior que a armazenada
            if (aux_comp > diferenca){
            	//printf("Trocou\n");
              //Diferença global é atualizada
              diferenca = aux_comp;
              //Armazenando as posições da maior diferença encontrada até o momento
              indice_i_maior = i;
              indice_j_maior = j;
              indice_i_menor = n;
              indice_j_menor = m;
              //Armazenando o valor da maior diferença encontrada até o momento
              valor_maior = matriz[i*tam+j];
              valor_menor = matriz[n*tam+m];
            }
          }
        }
      }
    }
  }

  // Corrigir os indices das matrizes
  indice_i_maior += minLeitura;
  indice_i_menor += minLeitura;

  // criar vetor de buffer
  int *buffer;
  MPI_Status status;

  // caso nao seja o processo 0
  if (meuRank != 0){

    // gere um buffer com os dados calculados no seu bloco
    buffer = (int *) malloc(sizeof(int)*TAMBUFFER);
    buffer[DIFERENCA] = diferenca;
    buffer[MENORI] = indice_i_menor;
    buffer[MAIORI] = indice_i_maior;
    buffer[MENORJ] = indice_j_menor;
    buffer[MAIORJ] = indice_j_maior;
    buffer[VALORMENOR] = valor_menor;
    buffer[VALORMAIOR] = valor_maior;

    // envie ao processo 0
    MPI_Send(buffer, TAMBUFFER, MPI_INT, 0, meuRank, MPI_COMM_WORLD);

  } else { // caso seja o processo 0

    for (i = 1; i < noProcessos; i++){ // para cada outro processo

      // receba o buffer criado pelo processo
      buffer = (int *) malloc(sizeof(int)*TAMBUFFER);
      MPI_Recv(buffer, TAMBUFFER, MPI_INT, i, i, MPI_COMM_WORLD, &status); 

      // caso ele represente uma solução melhor, substitua
      if (buffer[DIFERENCA] > diferenca){
        diferenca = buffer[DIFERENCA];
        indice_i_menor = buffer[MENORI];
        indice_i_maior = buffer[MAIORI];
        indice_j_menor = buffer[MENORJ];
        indice_j_maior = buffer[MAIORJ];
        valor_menor = buffer[VALORMENOR];
        valor_maior = buffer[VALORMAIOR];
      }

      free(buffer);
      buffer = NULL;
    }

    //Escrita dos resultados no arquivo de saída no padrão solicitado pelo exemplo
    printf("M[%d,%d]=%d M[%d,%d]=%d" ,indice_i_maior, indice_j_maior, valor_maior, indice_i_menor, indice_j_menor, valor_menor);
  }

  //Liberar a matriz
  //for(i = 0; i < (maxLeitura - minLeitura + 1); i++)
  //  free(matriz[i]);

  free(matriz);

  //Fechando os arquivos de entrada e saída
  fclose(arquivo_entrada);
  //fclose(arquivo_saida);
*/

  free(matriz);
  free(quantidadeElem);
  free(offsetsMatriz);

  // Fechando MPI
  MPI_Finalize();

  return 0;
}
