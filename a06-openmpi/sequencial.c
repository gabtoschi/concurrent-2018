#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <math.h>

// Based on Author: Ana Caroline Spengler

int main(void){

    //Criação e abertura dos arquivos de entrada e saída
    FILE *arquivo_entrada,*arquivo_saida;
    arquivo_entrada=fopen("numeros.txt","r");
    arquivo_saida=fopen("saida.txt","w+");

    //Tamanho da matriz
    int tam;
    //Variáveis auxiliares
    int i, j, n, m;

    //Leitura do tamanho da matriz
    fscanf(arquivo_entrada, "%d\n", &tam);

    //Matriz
    int **matriz=((int**)malloc(tam*sizeof(int*)));

    //Alocação da matriz
    for(i = 0; i < tam; i++)
        matriz[i]=((int*)malloc(tam*sizeof(int)));

    //Leitura da matriz
    for(i = 0; i < tam; i++){
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

  for (i = 0; i < tam; i++) {
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

  //Escrita dos resultados no arquivo de saída no padrão solicitado pelo exemplo
  fprintf(arquivo_saida, "M[%d,%d]=%d M[%d,%d]=%d" ,indice_i_maior, indice_j_maior, valor_maior, indice_i_menor, indice_j_menor, valor_menor);

  //Liberar a matriz
  for(i = 0; i < tam; i++)
    free(matriz[i]);

  free(matriz);

  //Fechando os arquivos de entrada e saída
  fclose(arquivo_entrada);
  fclose(arquivo_saida);

  return 0;
}
