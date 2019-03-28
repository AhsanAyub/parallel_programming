#include "stdio.h"
#include "stdlib.h"
#include "omp.h"
#include "malloc.h"

const long DOUBLES_PER_LOOP = 16; //4 avx registers in loop and 4 doubles per avx register

void asm_load(double* array, long size, long iters)
{
	size /= DOUBLES_PER_LOOP;
	
	asm volatile (
	"xor rax, rax"										"\n\t"
	".IterLoop%=:"										"\n\t"
		"inc rax"										"\n\t"
		"xor rbx, rbx"									"\n\t"
		"xor rcx, rcx"									"\n\t"
		".RowsLoop%=:"									"\n\t"
			"inc rbx"									"\n\t"
			"vmovapd ymm0, YMMWORD PTR [%[arr]+rcx]"	"\n\t"
			"vmovapd ymm1, YMMWORD PTR [%[arr]+rcx+32]"	"\n\t"
			"vmovapd ymm2, YMMWORD PTR [%[arr]+rcx+64]"	"\n\t"
			"vmovapd ymm3, YMMWORD PTR [%[arr]+rcx+96]"	"\n\t"
			"add rcx, 128"								"\n\t"
			"cmp rbx, %[len]"							"\n\t"
		"jne .RowsLoop%="								"\n\t"
		"cmp rax,%[iters]"								"\n\t"
	"jne .IterLoop%="									"\n\t"
	:
	:[arr] "r"(array), [len] "r"(size), [iters] "r"(iters)
	:"rax", "rbx", "rcx", "memory", "cc"
	);
}

void load(long size, long iters)
{	
	double* array = memalign(4096,size*sizeof(double));
	for(long i = 0; i < size; i++)
		array[i]=i;
	
	double start_time = omp_get_wtime();
	
	asm_load(array, size, iters);
	
	double end_time = omp_get_wtime();
	
	printf("%lu,%lu,%f\n",size,iters,end_time-start_time);
	
	free(array);
}

int main(int argc, char* argv[])
{
	long start = 16;
	long end = 1L<<24;
	long steps = 8;  //steps per binary order of magnitude
	long min_divisiblity = DOUBLES_PER_LOOP;
	long total_doubles_to_read = 1L<<32;
	
	for(long i = start; i < end; i*=2)
	{
		for(long j = 0; j < steps; j++)
		{
			long size = i*(j+steps)/steps;
			if(size % min_divisiblity == 0)
				load(size,total_doubles_to_read/size);
		}
	}
}