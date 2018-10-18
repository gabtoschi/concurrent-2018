/* 
--- GRUPO 2 ---
Ali Husseinat - 9292966
Andre Seiti Caniza - 9790923
Danilo da Costa Telles Téo - 9293626
Vitor Trevelin Xavier da Silva - 9791285
Murilo Bernard Theodoro - 8532130
Gabriel Toschi de Oliveira - 9763039

--- PCAM ---
Particionamento: 
Se a matriz tem ordem N, teremos N^2 tarefas que tem como dados um dos valores da matriz e calculam a maior diferença entre seu dado e dos seus vizinhos. Também temos uma tarefa que inicia as outras tarefas e uma que recebe os valores calculados pelas outras tarefas e define a maior.

Comunicação:
Cada tarefa tem que comunicar-se com as tarefas que contém os valores do seu vizinho, além de mandar o resultado para a tarefa que calcula a maior das diferenças.

Aglomeração:
Para reduzir o overhead de comunicação, que, nesse caso, é gigante, nós algomeramos tarefas da seguinte forma: cada processo ficará com um bloco de linhas proporcional à razão de N pelo número de processos. Também aglomeramos as tarefas de criar as outras tarefas e a tarefa final com a tarefa que lê as primeiras linhas. Um ponto que vale destacar é a replicação das linhas adjacentes a cada bloco para reduzir ainda mais a comunicação.

Mapeamento: O mapeamento foi feito em dois processos, o máximo permitido no uso do MPI pela máquina utilizada durante o desenvolvimento do algoritmo. Como parte do paradigma de passagem de mensagens, não há memória compartilhada entre os processos.
*/

#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>

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

    // calculando quantas linhas da matriz cada processo calculara
    int tamanhoBloco = tam / noProcessos;
    int restoBloco = tam % noProcessos;
    int minRange = 0, maxRange = 0;

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

    for(i = 0; i < (maxLeitura - minLeitura + 1); i++){
        for(j = 0; j < tam; j++)
            fscanf(arquivo_entrada, "%d\n", &(matriz[i*tam+j]));
    }

    for(i = 0; i < (maxLeitura - minLeitura + 1); i++){
        for(j = 0; j < tam; j++)
            printf("%d ", matriz[i*tam+j]);
        printf("\n");
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

  // Fechando MPI
  MPI_Finalize();

  return 0;
}
