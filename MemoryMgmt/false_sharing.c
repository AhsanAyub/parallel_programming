#include "omp.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("usage: %s [num_threads] <stride>\n",argv[0]);
		return 1;
	}
	
	int num_threads = atoi(argv[1]);
	int stride = 1;
	
	if(argc >= 3)
		stride = atoi(argv[2]);

	int* array = memalign(1024,num_threads*stride*sizeof(int));
	for(int i = 0; i < num_threads; i+=stride)
		array[i]=0;
	
	double start_time = omp_get_wtime();
	#pragma omp parallel num_threads(num_threads)
	{
		int index = omp_get_thread_num()*stride;
		for(int i = 0; i < 1e9; i++)
			array[index]++;
	}
	double end_time = omp_get_wtime();
	
	printf("Took: %f\n",end_time-start_time);
	
	return 0;
}
