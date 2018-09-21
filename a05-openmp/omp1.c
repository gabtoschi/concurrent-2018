/* compilar: gcc --o omp1 omp1.c -fopenmp */

#include <stdio.h>
#include <omp.h>

int main(){
	int x;

 	x = 2;
 	#pragma omp parallel num_threads(2) shared(x) 
 	{
 		if (omp_get_thread_num() == 0) {
 			x = 10;
 		} else {
 			/* Print 1: the following read of x has a race */
 			printf("1: Thread# %d: x = %d\n", omp_get_thread_num(),x );
 		}

 		#pragma omp barrier

 		if (omp_get_thread_num() == 0) {
 			/* Print 2 */
 			printf("2: Thread# %d: x = %d\n", omp_get_thread_num(),x );
 		} else {
 			/* Print 3 */
 			printf("3: Thread# %d: x = %d\n", omp_get_thread_num(),x );
 		}
 	}

 	return 0;
}