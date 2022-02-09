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

    // race variable
    int *race;

    // race lock
    pthread_mutex_t *race_Lock;

} arguments;

// create buffer
int buffer[3];

int buffer2[3];

// do work function
void *do_work(void *arg);

void *do_work2(void *arg);

// main method
int main(int argc, char *argv) 
{

    // create threads
    pthread_t threads[6];

    // create a lock for the buffer
    pthread_mutex_t lock_1;

    pthread_mutex_init( &lock_1, NULL );

    pthread_mutex_t lock_2;
    
    pthread_mutex_init( &lock_2, NULL );

    // initialize buffer to 0
    int buf_Size_1 = 0;

    int buf_Size_2 = 0;

    // initialize sequence tracking structs
    int seq_Total_1 = 0;

    int seq_Correct_1 = 0;

    int seq_Total_2 = 0;

    int seq_Correct_2 = 0;

    // race finish variable
    int race_Finish = 0;

    // race variable lock
    pthread_mutex_t race;

    pthread_mutex_init(&race, NULL);

    // build three thread arguments
    arguments *arg_1[3];

    for(int index = 0; index < 3; index++ )
    {

        arg_1[index] = calloc(1, sizeof(arguments));

        arg_1[index]->thread_Id = index+1;

        arg_1[index]->mutex = &lock_1;

        arg_1[index]->buf_Size = &buf_Size_1;

        arg_1[index]->seq_Total = &seq_Total_1;

        arg_1[index]->seq_Correct = &seq_Correct_1;

        arg_1[index]->race = &race_Finish;

        arg_1[index]->race_Lock = &race;

    }

    arguments *arg_2[3];

    for(int index = 0; index < 3; index++ )
    {

        arg_2[index] = calloc(1, sizeof(arguments));

        arg_2[index]->thread_Id = index+4;

        arg_2[index]->mutex = &lock_2;

        arg_2[index]->buf_Size = &buf_Size_2;

        arg_2[index]->seq_Total = &seq_Total_2;

        arg_2[index]->seq_Correct = &seq_Correct_2;
        
        arg_2[index]->race = &race_Finish;

        arg_2[index]->race_Lock = &race;

    }

    // create threads 1-3
    for(int index = 0; index < 3; index++ )
    {

        if(pthread_create(&threads[index], NULL, do_work, (void *)arg_1[index])) 
        {

            fprintf(stderr, "Error while creating thread %d.\n", index);

            exit(1);

        }
    }

    // create threads 4-6
    for(int index = 3; index < 6; index++ )
    {
        if(pthread_create(&threads[index], NULL, do_work2, (void *)arg_2[index-3])) 
        {

            fprintf(stderr, "Error while creating thread %d.\n", index);

            exit(1);

        }
    }

    // join threads
    for(int index = 0; index < 6; index++ )
    {

        if(pthread_join( threads[index], NULL)) 
        {

            fprintf( stderr, "Error while joining with child thread %d.\n", index);

            exit(1);
        
        }
    }
    
    // sequence print statements
    printf("Total sequences generated team1: %d\n", seq_Total_1);

    printf("Number of correct sequences team1: %d\n", seq_Correct_1);

    printf("Total sequences generated team2: %d\n", seq_Total_2);

    printf("Number of correct sequences team2: %d\n", seq_Correct_2);
    
    exit(0);
}

// do work function
void *do_work( void *arg ) 
{
    // arguments
    arguments *argument = (arguments*)arg;

    pthread_mutex_t *mutex = argument->mutex;

    pthread_mutex_t *race_Lock = argument->race_Lock;

    int *buf_Size = argument->buf_Size;

    int thread_Id = argument->thread_Id;

    int *total = argument->seq_Total;

    int *correct = argument->seq_Correct;

    int *race_Started = argument->race;
    

    // loop until we have done 10 correct iterations
    while( *race_Started == 0 && *correct < 10 ) {

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

                // winner?
                if( *correct == 10 )
                {

                    pthread_mutex_lock(race_Lock);

                    if( *race_Started == 0 )
                    {

                        printf("Team 1 Won!\n");
                        *race_Started = 1;

                    }

                    pthread_mutex_unlock(race_Lock);

                }
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

void *do_work2( void *arg ) 
{
    // arguments
    arguments *argument = (arguments*)arg;

    pthread_mutex_t *mutex = argument->mutex;

    pthread_mutex_t *race_Lock = argument->race_Lock;

    int *buf_Size = argument->buf_Size;

    int thread_Id = argument->thread_Id;

    int *total = argument->seq_Total;

    int *correct = argument->seq_Correct;

    int *race_Started = argument->race;
    

    // loop until we have done 10 correct iterations
    while( *race_Started == 0 && *correct < 10 ) {

        // thread locks
        pthread_mutex_lock(mutex);

        printf("\nMy id: %d\n", thread_Id);

        // increment buffer
        buffer2[*buf_Size] = thread_Id;

        *buf_Size += 1;

        // check if it was the final element in the buffer
        if(*buf_Size == 3)
        {
            if(buffer2[0] == 4 && buffer2[1] == 5 && buffer2[2] == 6)
            {

                printf("\n%d%d%d\n", buffer2[0], buffer2[1], buffer2[2]);

                *correct += 1;

                // winner?
                if( *correct == 10 )
                {

                    pthread_mutex_lock(race_Lock);
                    
                    if( *race_Started == 0 )
                    {

                        printf("Team 2 Won!\n");

                        *race_Started = 1;

                    }

                    pthread_mutex_unlock(race_Lock);

                }
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