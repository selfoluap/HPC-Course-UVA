!#/bin/bash
module load 2023
module load GCC/12.3.0
gcc -fopenmp -o sequential main_seq.c -lm