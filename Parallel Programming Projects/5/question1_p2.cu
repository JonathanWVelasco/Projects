#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <iostream>

// See values of N in assignment instructions.
#define N 100000
// N = { 100, 1000, 10000, 100000 }

// Do not change the seed, or your answer will not be correct
#define SEED 72

// For GPU implementation
#define BLOCKSIZE 1024

using namespace std;

struct pointData{
    double x;
    double y;
};
    
void generateDataset(struct pointData * data);
void warmUpGPU();
__global__ void getDistance(pointData *data, unsigned int *count, double *epsilon); // add parameters 


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
    data=(struct pointData *)malloc(sizeof(struct pointData) * N);
    printf("\nSize of dataset (MiB): %f",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
    generateDataset(data);

    omp_set_num_threads(1);

    double tstart = omp_get_wtime();

    //Write your code here:
    //The data you need to use is stored in the variable "data", 
    //which is of type pointData

    warmUpGPU();
    
    // distance counts
    struct pointData *devData;
    unsigned int *totalCount;
    unsigned int *dev_totalCount;
    double *devEpsilon;
    totalCount = (unsigned int *)malloc(sizeof(unsigned int));
    // dev_totalCount = (unsigned int *)malloc(sizeof(unsigned int));
    *totalCount = 0;

    // init error
    cudaError_t errCode=cudaSuccess;
    if(errCode != cudaSuccess){
        cout << "\nLast error: " << errCode << endl; 	
    }

    // allocate on the device: pointData
    errCode = cudaMalloc((struct pointData**)&devData, sizeof(struct pointData) * N);
    if(errCode != cudaSuccess) {
        cout << "\nError: point data error with code " << errCode << endl; 
    }

    // epsilon value
    errCode = cudaMalloc((double**)&devEpsilon, sizeof(double));
    if(errCode != cudaSuccess) {
        cout << "\nError: epsilon error with code " << errCode << endl; 
    }

    // epsilon value
    errCode = cudaMalloc((unsigned int**)&dev_totalCount, sizeof(unsigned int)*N);
    if(errCode != cudaSuccess) {
        cout << "\nError: count error with code " << errCode << endl; 
    }

    // copy data
    errCode = cudaMemcpy( devData, data, sizeof(pointData) * N, cudaMemcpyHostToDevice);
    if(errCode != cudaSuccess) {
        cout << "\nError: devData memcpy error with code " << errCode << endl; 
    }

    errCode = cudaMemcpy( dev_totalCount, totalCount, sizeof(unsigned int), cudaMemcpyHostToDevice);
    if(errCode != cudaSuccess) {
        cout << "\nError: devCounter memcpy error with code " << errCode << endl; 
    }

    errCode = cudaMemcpy( devEpsilon, (void *)&epsilon, sizeof(double), cudaMemcpyHostToDevice);
    if(errCode != cudaSuccess) {
        cout << "\nError: devEpsilon memcpy error with code " << errCode << endl; 
    }
    
    // calculate blocks
    const unsigned int totalBlocks = ceil(N * 1.0 / 1024.0); 

    // time kernel
    double kerStart = omp_get_wtime();

    // execute kernel
    getDistance<<<totalBlocks, BLOCKSIZE>>>(devData, dev_totalCount, devEpsilon);
    cudaDeviceSynchronize();

    // end kernel timer
    double kerEnd = omp_get_wtime();
    
    // copy count back out
    cudaMemcpy(totalCount, dev_totalCount, sizeof(unsigned int), cudaMemcpyDeviceToHost);

    // total end timer
	double tend = omp_get_wtime();

    // Display statements
    printf("\nEpsilon Value: %f", epsilon);
    printf("\nDataset Size: %d", N);
    printf("\nTotal number of points within epsilon: %u", *totalCount);
    printf("\nTotal time (s): %f", (tend - tstart));
    printf("\nKernal time (s): %f", (kerEnd - kerStart));

	free(data);
	printf("\n");
	return 0;
}


// Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData * data)
{
	// seed RNG
	srand(SEED);

	for (unsigned int i=0; i<N; i++){
		data[i].x=1000.0*((double)(rand()) / RAND_MAX);	
		data[i].y=1000.0*((double)(rand()) / RAND_MAX);	
	}
}

__global__ void getDistance(pointData *data, unsigned int *count, double *epsilon) {
    unsigned int tid = threadIdx.x + (blockIdx.x*BLOCKSIZE); 
    if(tid >= N){
        return;
    }
    atomicAdd(count, int(1)); // default add one for this point
    for(int i = tid + 1; i < N; i++){
        if(sqrt( (data[tid].x - data[i].x) * (data[tid].x - data[i].x) + 
          (data[tid].y - data[i].y) * (data[tid].y - data[i].y)  ) <= *epsilon){
            atomicAdd(count, int(2)); // double count to reduce
        }
    }
    return;
}


/// other warmup

__global__ void warmup(unsigned int * tmp) {
    if (threadIdx.x==0)
    *tmp=555;
    
    return;
}
    
    
    
void warmUpGPU(){
    printf("\nWarming up GPU for time trialing...\n");	
    unsigned int * dev_tmp;
    unsigned int * tmp;
    tmp=(unsigned int*)malloc(sizeof(unsigned int));
    *tmp=0;
    cudaError_t errCode=cudaSuccess;
    errCode=cudaMalloc((unsigned int**)&dev_tmp, sizeof(unsigned int));	
    if(errCode != cudaSuccess) {
        cout << "\nError: dev_tmp error with code " << errCode << endl; 
    }

    warmup<<<1,256>>>(dev_tmp);

    //copy data from device to host 
    errCode=cudaMemcpy( tmp, dev_tmp, sizeof(unsigned int), cudaMemcpyDeviceToHost);
    if(errCode != cudaSuccess) {
        cout << "\nError: getting tmp result form GPU error with code " << errCode << endl; 
    }

        printf("\ntmp (changed to 555 on GPU): %d",*tmp);

    cudaFree(dev_tmp);

    return;
}
    
