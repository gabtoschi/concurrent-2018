// Based on Author: John Burkardt

/*
GRUPO 4
Gabriel Toschi de Oliveira - 9763039
Bárbara Côrtes e Souza - 9763050
Victor Roberti Camolesi - 9791239
Edson Yudi Toma - 9791305
Danilo da Costa Telles Teo - 9293626
Igor Barbosa Grécia Lúcio - 9778821

Particionamento: cada tarefa consiste em determinar se um único número é primo. 
Comunicação: cada tarefa possui uma variável local, "prime", que indica se o número é primo ou não. Por fim, esses valores devem ser somados em uma tarefa específica pra tal.
Aglomeração: as tarefas são agrupadas em intervalos, de acordo com a faixa de números que estão checando. A soma é feita internamente em cada thread, utilizando a variável local "total", cujos valores são somados na tarefa específica de soma.
Mapeamento: o SO e a API OpenMP se encarrega dessa tarefa.
*/

#include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>

#include <omp.h>

int main ( int argc, char *argv[] );
void prime_number_sweep ( int n_lo, int n_hi, int n_factor );
int prime_number ( int n );

int main ( int argc, char *argv[] )  {
  int n_lo, n_hi, n_factor;

  n_lo = 5;
  n_hi = 500000;
  n_factor = 10;

  
  prime_number_sweep ( n_lo, n_hi, n_factor );

  printf ( "  Normal end of execution.\n" );
  
  
  return 0;
}// end main

void prime_number_sweep ( int n_lo, int n_hi, int n_factor )
/*  Purpose: PRIME_NUMBER_SWEEP does repeated calls to PRIME_NUMBER.
  Author: John Burkardt
  Parameters:
    Input, int N_LO, the first value of N.
    Input, int N_HI, the last value of N.
    Input, int N_FACTOR, the factor by which to increase N after
    each iteration.
  Example:      N  PRIME_NUMBER
                1           0
               10           4
              100          25
            1,000         168
           10,000       1,229
          100,000       9,592
        1,000,000      78,498
       10,000,000     664,579 */
{
  int i, n, primes;
  struct timeval t0, t1;
  long elapsed;

  
  printf ( "  Call PRIME_NUMBER to count the primes from %d to %d.\n", n_lo, n_hi );
  printf ( "         N        Pi    Time\n" );
  n = n_lo;
  while ( n <= n_hi )   {
    gettimeofday(&t0, 0);

    primes = prime_number ( n );
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;

    printf ( "  %8d  %8d  %d\n", n, primes, elapsed); //wtime );
 
    n = n * n_factor;
    
  }// end while
  
  return;
}// end prime_number_sweep

int prime_number ( int n )
/*   PRIME_NUMBER returns the number of primes between 1 and N.
  Author: John Burkardt
  Parameters:
    Input, int N, the maximum number to check.
    Output, int PRIME_NUMBER, the number of prime numbers up to N.  */
{
  int i, j, prime, total = 0;

  #pragma omp parallel num_threads(4) shared(n) private(i, j, prime)
  {
    #pragma omp for reduction(+: total) schedule(guided)
    for (i = 2; i <= n; i++){
      prime = 1;
      for ( j = 2; j*j <= i; j++ ) {
        if ( i % j == 0 ) {
          prime = 0;
          break;
        }
      } 
      total += prime;
    }
  }

  return total;
} 
