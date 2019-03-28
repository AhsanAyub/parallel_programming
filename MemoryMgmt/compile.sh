gcc -o false_sharing false_sharing.c -fopenmp -Wall -g 
gcc -o loading loading.c -fopenmp -Wall -g -masm=intel
gcc -o complicated_loading loading_complicated.c -fopenmp -Wall -g -masm=intel

gcc -o matrix_mult matrix_mult.c -fopenmp -Wall -g ../matrix/gemm_compare/lib/lib/libopenblas.a -I../matrix/gemm_compare/lib/include