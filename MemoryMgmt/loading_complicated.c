#include "stdio.h"
#include "stdlib.h"
#include "omp.h"
#include "malloc.h"

#define ELEMENT_SIZE 128 

void asm_load(double* array, long elements_per_row, long stride, long number_of_rows, long iters)
{
		stride *= ELEMENT_SIZE; //convert stride to units of bytes
	asm volatile (
	".IterLoop%=:"										"\n\t"
	"dec %[iters]"										"\n\t"
		"mov rcx, %[arr]"								"\n\t"
		"mov rax, %[num]"								"\n\t"
		".RowsLoop%=:"									"\n\t"
		"dec rax"										"\n\t"
			"xor rdx, rdx"								"\n\t"
			"xor rbx, rbx"								"\n\t"
			".InnerLoop%=:"								"\n\t"
			"inc rdx"									"\n\t"
				"vmovapd ymm0, YMMWORD PTR [rcx+rbx]"	"\n\t"
				"vmovapd ymm1, YMMWORD PTR [rcx+rbx+32]"	"\n\t"
				"vmovapd ymm1, YMMWORD PTR [rcx+rbx+64]"	"\n\t"
				"vmovapd ymm1, YMMWORD PTR [rcx+rbx+96]"	"\n\t"
				"add rbx, 128"							"\n\t"
			"cmp rdx, %[len]"							"\n\t"
			"jne .InnerLoop%="							"\n\t"
			"add rcx, %[stride]"						"\n\t"
		"cmp rax, 0"									"\n\t"
		"jne .RowsLoop%="								"\n\t"
	"cmp %[iters], 0"									"\n\t"
	"jne .IterLoop%="									"\n\t"
	:
	:[arr] "r"(array), [len] "r"(elements_per_row), [stride] "r"(stride), [num] "r"(number_of_rows), [iters] "r"(iters)
	:"rax", "rbx", "rcx", "rdx", "memory", "cc"
	);
}

void load(long elements_per_row, long stride, long number_of_rows, long total_bytes)
{	
	long iters = total_bytes/(elements_per_row*ELEMENT_SIZE*number_of_rows);

	long array_length = (stride>elements_per_row?stride:elements_per_row)*number_of_rows*ELEMENT_SIZE;//bytes
	double* array = memalign(4096,array_length);
	for(long i = 0; i < array_length/sizeof(double); i++)
		array[i]=i;
	
	double start_time = omp_get_wtime();
	
	asm_load(array, elements_per_row, stride, number_of_rows, iters);
	
	double end_time = omp_get_wtime();
	
	printf("%lu,%lu,%lu,%lu,%f\n",elements_per_row,stride,number_of_rows,iters,end_time-start_time);
	
	free(array);
}

int main(int argc, char* argv[])
{
	long start = 1;
	long end = 1L<<24;
	long steps = 8;  //steps per binary order of magnitude
	long min_divisiblity = 1;
	long total_bytes_to_read = 16*1024L*1024*1024;
	
	for(long i = start; i < end; i*=2)
	{
		for(long j = 0; j < steps; j++)
		{
			long size = i*(j+steps)/steps;
			if(size % min_divisiblity == 0)
			{
				load(1,size,8,total_bytes_to_read);
			}
		}
	}
}