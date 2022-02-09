#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 
#include <pthread.h>

#define SIZE 50
#define NUMTHREADS 2

double mat[SIZE][SIZE];

typedef struct arguments{
    int *rowCount;
    pthread_mutex_t *rowLock;
    double *timer; //
    int *counter; //
} arguments;

double do_crazy_computation(int i,int j);
void *do_work( void *arg );

int main(int argc, char **argv) {
  int i = 0;

  pthread_t threads[NUMTHREADS];

  pthread_mutex_t rowLock;
  pthread_mutex_init(&rowLock, NULL);

  double timer0, timer1;

  int counter0, counter1;
  counter0 = 0;
  counter1 = 0;

  arguments *args[NUMTHREADS];

  for( int k = 0; k < NUMTHREADS; k++ ){
    args[k] = calloc(1, sizeof(arguments));
    args[k]->rowCount = &i;
    args[k]->rowLock = &rowLock;
    if(k == 0){
        args[k]->counter = &counter0;
        args[k]->timer = &timer0;
    }else{
        args[k]->counter = &counter1;
        args[k]->timer = &timer1;
    }
  }

  double tstart = omp_get_wtime();

  for(int k = 0; k < NUMTHREADS; k++)
  {
      if(pthread_create(&threads[k], NULL, do_work, (void *)args[k])){
          fprintf(stderr, "Error while creating thread%d.\n", k);
          exit(1);
      }
  }

  for(int k = 0; k < NUMTHREADS; k++){
      if(pthread_join(threads[k], NULL)){
          fprintf(stderr, "Error while joining with child thread %d.\n", k);
          exit(1);
      }
  }

  double tend = omp_get_wtime();
  double elapsed = tend - tstart;
  printf("\nTotal time (sanity check): %f\n", elapsed);
  printf("Number of iterations for Thread1: %d\n", counter0);
  printf("Number of iterations for Thread2: %d\n", counter1);
  double loadImbal = fabs(timer1 - timer0);
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

void *do_work( void * arg ){

    arguments *args = (arguments *)arg;

    int *iterCounter = args->counter;
    int *rowCount = args->rowCount;
    double *timer = args->timer;
    pthread_mutex_t *rowLock = args->rowLock;

    int currentRow;
    int currentColumn;


    int bool = 1;


    double start = omp_get_wtime();

    while( bool == 1 ){

        pthread_mutex_lock(rowLock);
        if(*rowCount < SIZE){
            currentRow = *rowCount;
            *rowCount += 1;
            pthread_mutex_unlock(rowLock);

            *iterCounter += 1;

            for(currentColumn = 0; currentColumn < SIZE; currentColumn++){
                mat[currentRow][currentColumn] = do_crazy_computation(currentRow, currentColumn);
                fprintf(stderr,".");
            }

        }
        else{
            pthread_mutex_unlock(rowLock);
            bool = 0;
        }

    }
    
    double end = omp_get_wtime();
    *timer = end - start;
}

