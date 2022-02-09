//jaw645

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// create structs
typedef struct arguments 
{
    // thread id
    int thread_Id;

    // Total value for threads
    int *total;

    // counter
    int *counter;

    // number of iterations for threads
    int num_Iterations;

    // thread lock
    pthread_mutex_t *mutex;

    // lock conditional
    pthread_cond_t *conditional;

} arguments;


void *do_work( void *arg );

// main method
int main( int argc, char *argv )
{

    // create threads
    pthread_t threads[10];

    // initialize thread iterations
    int iterate = 22;

    // initialize total and counter
    int total = 0;

    int counter = 0;

    // create lock and conditional
    pthread_mutex_t lock; 

    pthread_mutex_init(&lock, NULL);

    pthread_cond_t conditional;

    pthread_cond_init(&conditional, NULL);
    

    // build 10 thread arguments
    arguments *args[10];

    for(int index = 0; index < 10; index++)
    {

        args[index] = calloc(1, sizeof(arguments));

        args[index]->total = &total;

        args[index]->mutex = &lock;

        args[index]->num_Iterations = iterate;

        args[index]->thread_Id = index;

        args[index]->counter = &counter;

        args[index]->conditional = &conditional;

    }

    // create threads
    for(int index = 0; index < 10; index++)
    {

        if(pthread_create(&threads[index], NULL, do_work, (void *)args[index])) 
        {

            fprintf(stderr, "Error while creating thread %d.\n", index);

            exit(1);

        }
    }

    // join threads
    for(int index = 0; index < 10; index++ )
    {

        if(pthread_join( threads[index], NULL)) 
        {

            fprintf( stderr, "Error while joining with child thread %d.\n", index);

            exit(1);

        }
    }

    // print total
    printf("Total: %d\n", total);

    exit(0);
} 


void *do_work(void *arg)
{
    // arguments
    arguments *args = (arguments *)arg;

    pthread_mutex_t *lock = args->mutex;

    pthread_cond_t *conditional = args->conditional;

    int *counter = args->counter;

    int thread_Id = args->thread_Id;

    int *total = args->total;

    int iterate = args->num_Iterations;

    // iterate loop
    int index = 0;

    while(index < iterate)
    {

        // lock
        pthread_mutex_lock(lock);

        while( *counter != thread_Id )
        {
            // conditional wait
            pthread_cond_wait( conditional, lock ); 

        }

        *counter = (*counter + 1) % 10;

        // increment total
        *total += thread_Id;

        // print statement
        printf("my num: %d, total: %d\n", thread_Id, *total);
        
        // increment the index
        index++;

        // conditional broadcast
        pthread_cond_broadcast(conditional);

        // unlock
        pthread_mutex_unlock(lock);

    }
}