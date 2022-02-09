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

    // buffer size
    int *buf_Size;

    // total sequences
    int *seq_Total;

    // correct sequences
    int *seq_Correct;

    // thread lock
    pthread_mutex_t *mutex;

} arguments;

// create buffer
int buffer[3];

// do work function
void *do_work(void *arg);

// main method
int main(int argc, char *argv) 
{

    // create threads
    pthread_t threads[3];

    // create a lock for the buffer
    pthread_mutex_t lock;

    pthread_mutex_init( &lock, NULL );

    // initialize buffer to 0
    int buf_Size = 0;

    // initialize sequence tracking structs
    int seq_Total = 0;

    int seq_Correct = 0;

    // build three thread arguments
    arguments *arg[3];

    for(int index = 0; index < 3; index++ )
    {

        arg[index] = calloc(1, sizeof(arguments));

        arg[index]->thread_Id = index+1;

        arg[index]->mutex = &lock;

        arg[index]->buf_Size = &buf_Size;

        arg[index]->seq_Total = &seq_Total;

        arg[index]->seq_Correct = &seq_Correct;

    }

    // create threads
    for(int index = 0; index < 3; index++ )
    {

        if(pthread_create(&threads[index], NULL, do_work, (void *)arg[index])) 
        {

            fprintf(stderr, "Error while creating thread %d.\n", index);

            exit(1);

        }
    }

    // join threads
    for(int index = 0; index < 3; index++ )
    {

        if(pthread_join( threads[index], NULL)) 
        {

            fprintf( stderr, "Error while joining with child thread %d.\n", index);

            exit(1);
        
        }
    }
    
    // sequence print statements
    printf("Total sequences generated: %d\n", seq_Total);

    printf("Number of correct sequences: %d\n", seq_Correct);
    
    exit(0);
}

// do work function
void *do_work( void *arg ) 
{
    // arguments
    arguments *argument = (arguments*)arg;

    pthread_mutex_t *mutex = argument->mutex;

    int *buf_Size = argument->buf_Size;

    int thread_Id = argument->thread_Id;

    int *total = argument->seq_Total;

    int *correct = argument->seq_Correct;
    

    // loop until we have done 10 correct iterations
    while( *correct < 10 ) {

        // thread locks
        pthread_mutex_lock(mutex);

        printf("\nMy id: %d\n", thread_Id);

        // increment buffer
        buffer[*buf_Size] = thread_Id;

        *buf_Size += 1;

        // check if it was the final element in the buffer
        if(*buf_Size == 3)
        {
            if(buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3)
            {

                printf("\n%d%d%d\n", buffer[0], buffer[1], buffer[2]);

                *correct += 1;
            }

            *total += 1;

            *buf_Size = 0;
            
        }

        // thread unlocks
        pthread_mutex_unlock(mutex);

        // sleep
        usleep(500000);
    }

    return NULL;

}