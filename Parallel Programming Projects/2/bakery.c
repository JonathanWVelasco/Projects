#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void random_sleep(double a, double b);

//ADDITIONAL CODE
void *do_work( void *arg );
//

#define NUM_ITERATIONS 10
#define NUM_LEFT_OVEN_MITTS 3
#define NUM_RIGHT_OVEN_MITTS 3

//Helper function to sleep a random number of microseconds
//picked between two bounds (provided in seconds)
//pass 0.2 and 0.5 into this function as arguments
void random_sleep(double lbound_sec, double ubound_sec) {
   int num_usec;
   num_usec = lbound_sec*1000000 + 
              (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX)); 
   usleep(num_usec);
   return; 
}

// ADDITIONAL CODE
enum bakerType
{

  leftOvenMittsBaker = 0,

  rightOvenMittsBaker = 1,

  cautiousBaker = 2

};

typedef struct arguments 
{

  enum bakerType type; 

  int id;

  int iterations;

  int *leftOvenMittsAvailable;

  int *rightOvenMittsAvailable;

  pthread_mutex_t *leftOvenMittsLock;

  pthread_mutex_t *rightOvenMittsLock;

  pthread_cond_t *leftOvenMittsCond;

  pthread_cond_t *rightOvenMittsCond;

} arguments;
//



/*
 * Main function
 */
int main(int argc, char **argv) {

  int num_left_handed_bakers;
  int num_right_handed_bakers;
  int num_cautious_bakers;
  int seed;

  /* Process command-line arguments */
  if (argc != 5) {
    fprintf(stderr,"Usage: %s <# left-handed bakers> <# right-handed bakers> <# cautious bakers> <seed>\n",argv[0]);
    exit(1);
  }

  if ((sscanf(argv[1],"%d",&num_left_handed_bakers) != 1) || 
      (sscanf(argv[2],"%d",&num_right_handed_bakers) != 1) || 
      (sscanf(argv[3],"%d",&num_cautious_bakers) != 1) || 
      (sscanf(argv[4],"%d",&seed) != 1) ||
      (num_left_handed_bakers < 1) ||
      (num_right_handed_bakers < 1) ||
      (num_cautious_bakers < 1) ||
      (seed < 0)) {
    fprintf(stderr,"Invalid command-line arguments... Aborting\n");
    exit(1);
  }
 
  /* Seed the RNG */
  srand(seed);

// IMPLEMENT CODE HERE

  int totalWorkers = num_left_handed_bakers + num_right_handed_bakers + num_cautious_bakers;

  int iterations = NUM_ITERATIONS;

  int leftOvenMittsAvailableNUM = NUM_LEFT_OVEN_MITTS;

  int rightOvenMittsAvailableNUM = NUM_RIGHT_OVEN_MITTS;


  pthread_t threads[totalWorkers];
  

  pthread_mutex_t leftOvenMittsLock;

  pthread_mutex_t rightOvenMittsLock;
  
  pthread_mutex_init(&leftOvenMittsLock, NULL);

  pthread_mutex_init(&rightOvenMittsLock, NULL);


  pthread_cond_t leftOvenMittsAvailable;

  pthread_cond_t rightOvenMittsAvailable;

  pthread_cond_init(&leftOvenMittsAvailable, NULL);

  pthread_cond_init(&rightOvenMittsAvailable, NULL);

  arguments *args[totalWorkers];

  for(int i = 0; i < totalWorkers; i++) 
  {

    args[i] = calloc(1, sizeof(arguments));

    args[i]->iterations = NUM_ITERATIONS;

    args[i]->leftOvenMittsAvailable = &leftOvenMittsAvailableNUM;

    args[i]->rightOvenMittsAvailable = &rightOvenMittsAvailableNUM;

    args[i]->leftOvenMittsLock = &leftOvenMittsLock;

    args[i]->rightOvenMittsLock = &rightOvenMittsLock;

    args[i]->leftOvenMittsCond = &leftOvenMittsAvailable;

    args[i]->rightOvenMittsCond = &rightOvenMittsAvailable;


    if( i < num_left_handed_bakers )
    {

      args[i]->type = leftOvenMittsBaker;

      args[i]->id = i;

    }

    else if( i < (num_left_handed_bakers + num_right_handed_bakers) )
    {

      args[i]->type = rightOvenMittsBaker;

      args[i]->id = i - num_left_handed_bakers;

    }
    else
    {

      args[i]->type = cautiousBaker;

      args[i]->id = i - num_left_handed_bakers - num_right_handed_bakers;

    }

  }

  for(int i = 0; i < totalWorkers; i++ )
  {

    if(pthread_create(&threads[i], NULL, do_work, (void *)args[i])) 
    {

      fprintf(stderr, "Error while creating thread %d.\n", i);

      exit(1);
      
    }

  }

  for(int i = 0; i < totalWorkers; i++ )
  {

    if(pthread_join( threads[i], NULL)) 
    {

      fprintf( stderr, "Error while joining with child thread %d.\n", i);

      exit(1);

    }
  }

  exit(0);
}

void *do_work( void *arg ){

  arguments *args = (arguments *)arg;


  enum bakerType type = args->type;

  int iterations = args->iterations;

  int id = args->id;

  int *leftOvenMittsAvailable = args->leftOvenMittsAvailable;

  int *rightOvenMittsAvailable = args->rightOvenMittsAvailable;

  pthread_mutex_t *leftOvenMittsLock = args->leftOvenMittsLock;

  pthread_mutex_t *rightOvenMittsLock = args->rightOvenMittsLock;

  pthread_cond_t *leftOvenMittsCond = args->leftOvenMittsCond;

  pthread_cond_t *rightOvenMittsCond = args->rightOvenMittsCond;

  int currIteration = 0;

  int lBound = .2;

  int uBound = .5;

  switch( args->type )
  {

    case leftOvenMittsBaker:

      while( currIteration < iterations )
      {


        printf("[Left-handed baker %d] is working...\n", id);

        random_sleep(lBound, uBound);


        printf("[Left-handed baker %d] wants a left-handed mitt...\n", id); 

        pthread_mutex_lock(leftOvenMittsLock);

        while(*leftOvenMittsAvailable == 0)
        {

          pthread_cond_wait(leftOvenMittsCond, leftOvenMittsLock);

        }

        printf("[Left-handed baker %d] has got a left-handed mitt...\n", id);

        *leftOvenMittsAvailable -= 1;

        pthread_mutex_unlock(leftOvenMittsLock);

        printf("[Left-handed baker %d] has put cookies in the oven and is waiting...\n", id);

        random_sleep(lBound, uBound);

        printf("[Left-handed baker %d] has taken cookies out of the oven...\n", id);

        pthread_mutex_lock(leftOvenMittsLock);

        printf("[Left-handed baker %d] has put back a left-handed mitt...\n", id);

        *leftOvenMittsAvailable +=1;

        pthread_cond_signal(leftOvenMittsCond);

        pthread_mutex_unlock(leftOvenMittsLock);

        currIteration++;

      }

      break;

    case rightOvenMittsBaker:

      while( currIteration < iterations )
      {

        printf("[Right-handed baker %d] is working...\n", id);

        random_sleep(lBound, uBound);

        printf("[Right-handed baker %d] wants a right-handed mitt...\n", id); 

        pthread_mutex_lock(rightOvenMittsLock);

        while(*rightOvenMittsAvailable == 0)
        {

          pthread_cond_wait(rightOvenMittsCond, rightOvenMittsLock);

        }

        printf("[Right-handed baker %d] has got a right-handed mitt...\n", id);

        *rightOvenMittsAvailable -= 1;

        pthread_mutex_unlock(rightOvenMittsLock);

        printf("[Right-handed baker %d] has put cookies in the oven and is waiting...\n", id);

        random_sleep(lBound, uBound);

        printf("[Right-handed baker %d] has taken cookies out of the oven...\n", id);

        pthread_mutex_lock(rightOvenMittsLock);

        printf("[Right-handed baker %d] has put back a right-handed mitt...\n", id);

        *rightOvenMittsAvailable +=1;

        pthread_cond_signal(rightOvenMittsCond);

        pthread_mutex_unlock(rightOvenMittsLock);

        currIteration++;

      }

      break;

    case cautiousBaker:

      while( currIteration < iterations )
      {

        printf("[Cautious baker %d] is working...\n", id);

        random_sleep(lBound, uBound);

        printf("[Cautious baker %d] wants a left-handed mitt...\n", id);


        pthread_mutex_lock(leftOvenMittsLock);

        while(*leftOvenMittsAvailable == 0)
        {

          pthread_cond_wait(leftOvenMittsCond, leftOvenMittsLock);

        }

        printf("[Cautious baker %d] has got a left-handed mitt...\n", id);

        *leftOvenMittsAvailable -= 1;

        pthread_mutex_unlock(leftOvenMittsLock);

        printf("[Cautious baker %d] wants a right-handed mitt...\n", id); 

        pthread_mutex_lock(rightOvenMittsLock);

        while(*rightOvenMittsAvailable == 0)
        {
          
          pthread_cond_wait(rightOvenMittsCond, rightOvenMittsLock);

        }

        printf("[Cautious baker %d] has got a right-handed mitt...\n", id);

        *rightOvenMittsAvailable -= 1;

        pthread_mutex_unlock(rightOvenMittsLock);

        printf("[Cautious baker %d] has put cookies in the oven and is waiting...\n", id);

        random_sleep(lBound, uBound);

        printf("[Cautious baker %d] has taken cookies out of the oven...\n", id);

        pthread_mutex_lock(leftOvenMittsLock);

        printf("[Cautious baker %d] has put back a left-handed mitt...\n", id);

        *leftOvenMittsAvailable +=1;

        pthread_cond_signal(leftOvenMittsCond);

        pthread_mutex_unlock(leftOvenMittsLock);

        pthread_mutex_lock(rightOvenMittsLock);

        printf("[Cautious baker %d] has put back a right-handed mitt...\n", id);

        *rightOvenMittsAvailable +=1;

        pthread_cond_signal(rightOvenMittsCond);

        pthread_mutex_unlock(rightOvenMittsLock);

        currIteration++;

      }

      break;

  }

  return NULL;

}
//
