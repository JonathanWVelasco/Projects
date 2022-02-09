#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define SIZE 50

double do_crazy_computation(int i,int j);

int main(int argc, char **argv) {
  double mat[SIZE][SIZE];
  int i,j;

  omp_set_num_threads(2);

  int tid;
  double timer0, timer1, start, end;
  double tstart=omp_get_wtime();

  //modify code here
  #pragma omp parallel shared(mat) private(i, j, start, end, tid) 
  {
    start = omp_get_wtime();
    
    #pragma omp for nowait schedule(dynamic) 
    for (i=0;i<SIZE;i++) { /* loop over the rows */
        for (j=0;j<SIZE;j++) {  /* loop over the columns */
        mat[i][j] = do_crazy_computation(i,j);
        fprintf(stderr,".");
        } 
    }

    end = omp_get_wtime();
    tid=omp_get_thread_num();
    if(tid == 0){
        timer0 = end-start;
    }
    else{
        timer1 = end-start;
    }
  }

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  double loadImbal = fabs(timer1 - timer0);
  printf("\nTotal Time (sanity check): %f\n",elapsed);
  printf("Time Thread1: %f\n", timer0);
  printf("Time Thread2: %f\n", timer1);
  printf("Load Imbalance: %f\n", loadImbal);

  exit(0);
}

//Crazy computation
double do_crazy_computation(int x,int y) {
   int iter;
   double value=0.0;

   for (iter = 0; iter < 5*x*x*x+1 + y*y*y+1; iter++) {
     value +=  (cos(x*value) + sin(y*value));
   }
  return value;
}