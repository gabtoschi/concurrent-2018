#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>

/*
	Participantes:
	Bárbara Côrtes e Souza			9763050
	Bruno Flávio Ferreira				9791330
	João Pedro Sousa dos Reis		9293373
	Leonardo de Souza Lemes			8941126
	Lucas Fernandes Turci  			9763085
	Rafael Amaro Rolfsen				9038648

	Grupo 5

	
	PCAM:

	particionamento: cada particao sera um unico elemento da matriz, é a menor particao possivel, o que confere mais flexibilidade ao codigo

	comunicacao: todas as particoes precisam fazer, no pior caso, 8 comunicacoes. cada elemento deve comunicar-se com seus vizinhos para realizar os calculos

	aglomeracao: as particoes serao aglomeradas em retangulos de aproximadamente n/p linhas cada. asssim, a nova comunicacao sera apenas entre os retangulos vizinhos (cima e baixo)

	mapeamento: mapeamos cada tarefa para um processador

*/

// Based on Author: Ana Caroline Spengler

int main(int argc, char *argv[]){
	int nproc, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Auxiliar para comparar o valor da diferenca entre os vizinhos
	int aux_comp;
	//Variável para guardar a maior diferenca encontrada
	int diferenca = INT_MIN;
	//Variáveis para armazenar os indices da maior diferença encontrada
	int indice_i_maior, indice_j_maior, indice_i_menor, indice_j_menor;
	//Variáveis para armazenar os valores da maior diferença encontrada
	//Poderia utilizar apenas os indices armazenados, colocados aqui por questões de clareza
	int valor_maior, valor_menor;
	
	//Tamanho da matriz
	int tam; 
	//Variáveis auxiliares
	int i, j, n, m;

	int localTam;
	int inicio;
	int fim;

		//Criação e abertura dos arquivos de entrada e saída
		FILE *arquivo_entrada;


	int ** mat;
	if(rank == 0) {
		arquivo_entrada=fopen("numeros.txt","r");

		//Leitura do tamanho da matriz
		fscanf(arquivo_entrada, "%d\n", &tam);

		//Matriz
		mat=((int**)malloc(tam*sizeof(int*)));

		//Alocação da Matriz
		for(i = 0; i < tam; i++){
			mat[i] = (int *) malloc(sizeof(int) * tam);
		}
	 
		//Leitura da matriz
		for(i = 0; i < tam; i++){
				for(j = 0; j < tam; j++)
						fscanf(arquivo_entrada, "%d\n", &(mat[i][j]));
		}
		localTam = tam/nproc + (rank < tam%nproc);  

		for(i = 1; i < nproc; ++i) {
			// processo i
			int st = i * (tam / nproc) + (i < tam % nproc ? i : tam % nproc);
			int localTam = tam/nproc + (i < tam%nproc);
			//st *= tam;

			MPI_Send(&tam, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for(j = 0; j < localTam; j++)
				MPI_Send(mat[st + j], tam, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	
	}

	if(rank != 0) {
		MPI_Recv(&tam, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		localTam = tam/nproc + (rank < tam%nproc);  
		mat = (int **) malloc(localTam * sizeof(int*));
		

		for(i = 0; i < localTam; i++){
			mat[i] = (int *) malloc(sizeof(int) * tam);
			MPI_Recv(mat[i], tam, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	/*
	if(rank%2 == 0 && rank) {
		MPI_Send(mat, tam, MPI_INT, rank-1, 0, MPI_COMM_WORLD);
	}*/

		int *ant = (int*) malloc(sizeof(int) * tam);
		int aux_mat;
		if(rank != 0){
			MPI_Recv(ant, tam, MPI_INT, rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if(rank != nproc -1)
			MPI_Send(mat[localTam -1], tam, MPI_INT, rank + 1, 2, MPI_COMM_WORLD);

		for (i = 0; i < localTam; i++) {
			for (j = 0; j < tam; j++) {

				//Avalia a diferenca entre o elemento matriz[i][j] e seus vizinhos
				for (n = i - 1; n < i + 1; n++) {
					for (m = j - 1; m < j + 2; m++) {
						//Se não for a borda da matriz, tire a diferenca
						if ( (m >= 0 && m < tam)){
							if(rank == 0 && n < 0)
								continue;

							// n = -1 => precisa comparar com a ultima linha do processo anterior
							if(n < 0)
								aux_mat = ant[m];
							else
								aux_mat = mat[n][m];

							//Módulo da diferenca entre o elemento matriz[i][j] e seu vizinho
							aux_comp = abs(mat[i][j] - aux_mat);
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
								valor_maior = mat[i][j];
								valor_menor = aux_mat;
							}
						}
					}
				}
			}
		}

		if(rank != 0){
			MPI_Send(&valor_maior, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
			MPI_Send(&valor_menor, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
			MPI_Send(&indice_i_maior, 1, MPI_INT, 0, 5, MPI_COMM_WORLD);
			MPI_Send(&indice_j_maior, 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
			MPI_Send(&indice_i_menor, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
			MPI_Send(&indice_j_menor, 1, MPI_INT, 0, 8, MPI_COMM_WORLD);
		}	



	if (rank == 0){
		int valores_maior[nproc];
		int valores_menor[nproc];
		int indices_i_maior[nproc];
		int indices_j_maior[nproc];
		int indices_i_menor[nproc];
		int indices_j_menor[nproc];

		valores_maior[0] = valor_maior;
		valores_menor[0] = valor_menor;
		indices_i_menor[0] = indice_i_menor;
		indices_i_maior[0] = indice_i_maior;
		indices_j_menor[0] = indice_j_menor;
		indices_j_maior[0] = indice_j_maior;



	//receber de todo mundo
		for(i = 1; i < nproc; i++){
			MPI_Recv(&valores_maior[i], 1, MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&valores_menor[i], 1, MPI_INT, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&indices_i_maior[i], 1, MPI_INT, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&indices_j_maior[i], 1, MPI_INT, i, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&indices_i_menor[i], 1, MPI_INT, i, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&indices_j_menor[i], 1, MPI_INT, i, 8, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		}
	//calcular resultado
		int pos = 0;
		for(i = 1; i < nproc; i++){
			aux_comp = abs(valores_maior[i] - valores_menor[i]);
			if(aux_comp > diferenca){
				diferenca = aux_comp;
				pos = i;
			}
		}

		
		int st = pos * (tam / nproc) + (pos < tam % nproc ? pos : tam % nproc);
		indices_i_menor[pos] += st;
		indices_i_maior[pos] += st;

		//Escrita dos resultados no arquivo de saída no padrão solicitado pelo exemplo
		if(valores_maior[pos] >= valores_menor[pos])
			printf("M[%d,%d]=%d M[%d,%d]=%d\n" ,indices_i_maior[pos], indices_j_maior[pos], valores_maior[pos], indices_i_menor[pos], indices_j_menor[pos], valores_menor[pos]);
		else
			printf("M[%d,%d]=%d M[%d,%d]=%d\n" ,indices_i_menor[pos], indices_j_menor[pos], valores_menor[pos], indices_i_maior[pos], indices_j_maior[pos], valores_maior[pos]);

		//Liberar a matriz
		for(i = 0; i < tam; i++)
			free(mat[i]);

		free(mat);

		//Fechando os arquivos de entrada e saída
		fclose(arquivo_entrada);

	}

	if(rank != 0){
		for(i = 0; i < localTam; i++)
			free(mat[i]);

		free(mat);
	}

	free(ant);

	MPI_Finalize();

	return 0;
}
