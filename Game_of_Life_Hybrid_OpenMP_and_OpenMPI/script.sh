#!/bin/bash
#SBATCH --nodes=4
#SBATCH --time=01:00:00
#SBATCH --cpus-per-task=16

mpirun -pernode ./hybrid intput.txt 16 600 output_4_16.txt
