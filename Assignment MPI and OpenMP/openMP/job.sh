#!/bin/bash
#SBATCH --job-name="gol"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --time=02:00:00
#SBATCH --partition=thin_course
#SBATCH --output=gol_%j.out
#SBATCH --error=gol_%j.err
module purge
module load 2023
module load GCC/12.3.0
module load OpenMPI/4.1.5-GCC-12.3.0

echo "OpenMP parallelism"
for i in 128 64 32 16 8 4 2 1
do
export OMP_NUM_THREADS=$i
echo "CPUS: " $OMP_NUM_THREADS
echo "CPUS: " $OMP_NUM_THREADS >&2
./gol 0 > gol_$i.out
done
echo "DONE "