#include "cblas.h"
#include "omp.h"
#include "stdlib.h"
#include "stdio.h"


void matrix_multiply(int size)
{
	double* A = malloc(size*size*sizeof(double));
	double* B = malloc(size*size*sizeof(double));
	double* C = malloc(size*size*sizeof(double));
	
	for(long i = 0; i < size*size; i++)
	{
		A[i]=i;
		B[i]=i;
		C[i]=0;
	}
	
	double start = omp_get_wtime();
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1, A, size, B, size, 0, C, size);
	double end = omp_get_wtime();
	
	printf("%d: %f\n",size, end-start);
}

int main()
{
	matrix_multiply(1000);
	matrix_multiply(2000);
	matrix_multiply(5000);
	matrix_multiply(7000);
	matrix_multiply(10000);
}