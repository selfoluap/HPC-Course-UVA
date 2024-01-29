#!/bin/bash
#SBATCH --job-name="game_of_life"
#SBATCH --nodes=1
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=32
#SBATCH --time=00:10:00
#SBATCH --partition=thin_course
rm -f game_of_life
module load 2023
module load GCC/12.3.0
module load OpenMPI/4.1.5-GCC-12.3.0
mpicc -fopenmp -o game_of_life main.c -lm

echo "Use the power of threads/processes"
echo


for ncores in 64, 128
do
export OMP_NUM_THREADS=$ncores
echo "CPUS: " $OMP_NUM_THREADS
echo "CPUS: " $OMP_NUM_THREADS >&2
./game_of_life 3000 5000 2 > test$ncores.txt
echo "DONE "
done


