//compilation instructions/examples:
//gcc -fopenmp point_epsilon_starter.c -o point_epsilon_starter
//sometimes you need to link against the math library with -lm:
//gcc -fopenmp point_epsilon_starter.c -lm -o point_epsilon_starter

//math library needed for the square root

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "omp.h"

//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72


typedef struct pointData{
double x;
double y;
} pointData;



void generateDataset(struct pointData * data);
double getDistance(pointData *p1, pointData *p2);


int main(int argc, char *argv[])
{
	
	
	//Read epsilon distance from command line
	if (argc!=2)
	{
	printf("\nIncorrect number of input parameters. Please input an epsilon distance.\n");
	return 0;
	}
	
	
	char inputEpsilon[20];
	strcpy(inputEpsilon,argv[1]);
	double epsilon=atof(inputEpsilon);
	
	

	//generate dataset:
	struct pointData * data;
	data=(struct pointData*)malloc(sizeof(struct pointData)*N);
	printf("\nSize of dataset (MiB): %f",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
	generateDataset(data);


	//change OpenMP settings:
	// omp_set_num_threads(1); // use 1 for sequential run
    omp_set_num_threads(4); // use this for parallelized


    double tstart=omp_get_wtime();
	

	//Write your code here:
	//The data you need to use is stored in the variable "data", 
	//which is of type pointData

    int i, j;
    i = j = 0;

    int sum = N;

    #pragma omp parallel for shared(sum, data) private(i, j) schedule(dynamic)
    for( i = 0; i < N; i++ ) {
        for( j = i + 1; j < N; j++ ) {
            if( fabs(data[i].x - data[j].x) <= epsilon && fabs(data[i].y - data[j].y) <= epsilon )
            {
                if(getDistance(&data[i], &data[j]) <= epsilon ) {
                    #pragma omp critical 
                    {
                        sum += 2;
                    }
                }
            }

            
        }
    }

	
	

	double tend=omp_get_wtime();

	printf("\nTotal time (s): %f",tend-tstart);
    printf("\nThe sum is: %d\n", sum);

	free(data);
	printf("\n");
	return 0;
}


//Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData * data)
{

	//seed RNG
	srand(SEED);


	for (unsigned int i=0; i<N; i++){
		data[i].x=1000.0*((double)(rand()) / RAND_MAX);	
		data[i].y=1000.0*((double)(rand()) / RAND_MAX);	
	}
}


// function for computing the distance
double getDistance( pointData *p1, pointData *p2){
    return( sqrt( pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2) ) );
}

