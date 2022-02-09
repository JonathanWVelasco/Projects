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

    // counter
    int *counter;

    // tracks thread turns
    int *thread_Tracker;

    // thread lock
    pthread_mutex_t *mutex;

    // lock conditional
    pthread_cond_t *conditional;

} arguments;

void *increment_work( void *arg );

void *decrement_work(void *arg);

// main method
int main( int argc, char *argv )
{

    // create threads
    pthread_t threads[2];

    // initialize tracker and counter
    int counter = 0;

    int thread_Tracker = 0;

    // create lock and conditional
    pthread_mutex_t lock; 

    pthread_mutex_init(&lock, NULL);

    pthread_cond_t conditional;

    pthread_cond_init(&conditional, NULL);
    

    // build thread arguments
    arguments *args[2];

    for(int index = 0; index < 2; index++)
    {
        
        args[index] = calloc(1, sizeof(arguments));

        args[index]->mutex = &lock;

        args[index]->thread_Id = index;

        args[index]->counter = &counter;

        args[index]->conditional = &conditional;

        args[index]->thread_Tracker = &thread_Tracker;

    }

    // create threads
    if(pthread_create(&threads[0], NULL, increment_work, (void *)args[0])) 
    {

        fprintf(stderr, "Error while creating thread 0.\n");

        exit(1);

    }

    if(pthread_create(&threads[1], NULL, decrement_work, (void *)args[1])) 
    {

        fprintf(stderr, "Error while creating thread 1.\n");

        exit(1);

    }
    
    // join threads
    for(int index = 0; index < 2; index++ )
    {
        if(pthread_join( threads[index], NULL)) 
        {

            fprintf( stderr, "Error while joining with child thread %d.\n", index);

            exit(1);

        }
    }

    exit(0);
} 


void *increment_work(void *arg)
{

    // arguments
    arguments *args = (arguments *)arg;

    pthread_mutex_t *lock = args->mutex;

    pthread_cond_t *conditional = args->conditional;
    
    int *counter = args->counter;

    int *thread_Tracker = args->thread_Tracker;

    int thread_Id = args->thread_Id;
    
    // iterate loop
    int index_1 = 0;

    while(index_1 < 3)
    {

        int index_2 = 0;

        while( index_2 < 10 )
        {

            // lock
            pthread_mutex_lock(lock);
            
            // thread execute conditional
            while( *thread_Tracker == 1 )
            {

                pthread_cond_wait( conditional, lock );

            }
            
            // decrement counter print counter
            *counter += 1;

            printf("Count is now (inc fn): %d\n", *counter);

            // increment tracker checker
            if(*counter == 10 )
            {

                *thread_Tracker += 1;

            }

            index_2++;

            // conditional signal
            pthread_cond_signal(conditional);

            // unlock
            pthread_mutex_unlock(lock);

            usleep(500000);

        }

        index_1++;
    }

    return NULL;

}

void *decrement_work(void *arg)
{

    // arguments
    arguments *args = (arguments *)arg;

    pthread_mutex_t *lock = args->mutex;

    pthread_cond_t *conditional = args->conditional;

    int *counter = args->counter;

    int *thread_Tracker = args->thread_Tracker;

    int thread_Id = args->thread_Id;

    // iterate loop
    int index_1 = 0;

    while(index_1 < 2)
    {

        int index_2 = 0;

        while( index_2 < 10 )
        {

            // lock
            pthread_mutex_lock(lock);
            
            // thread execute conditional
            while( *thread_Tracker == 0 )
            {

                pthread_cond_wait( conditional, lock );

            }
            
            // decrement counter print counter
            *counter -= 1;

            printf("Count is now (dec fn): %d\n", *counter);

            // decrement tracker checker
            if(*counter == 0)
            {

                *thread_Tracker -= 1;

            }

            index_2++;

            // conditional signal
            pthread_cond_signal(conditional);

            // unlock
            pthread_mutex_unlock(lock);

            usleep(500000);

        }

        index_1++;

    }

    return NULL;

}