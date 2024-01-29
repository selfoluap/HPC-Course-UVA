#!/bin/bash
#SBATCH --job-name="game_of_life"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --time=00:10:00
#SBATCH --partition=thin_course
rm -f game_of_life
module load 2023
module load GCC/12.3.0
module load OpenMPI/4.1.5-GCC-12.3.0
mpicc -fopenmp -o game_of_life main.c -lm

./game_of_life 30 50 2 > test.txt
