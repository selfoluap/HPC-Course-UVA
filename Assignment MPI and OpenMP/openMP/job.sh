#!/bin/bash
#SBATCH --job-name="pi"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --time=00:10:00
#SBATCH --partition=thin_course
#SBATCH --output=gof_%j.out
#SBATCH --error=gof_%j.err
module purge
module load 2023
module load GCC/12.3.0
module load OpenMPI/4.1.5-GCC-12.3.0

echo "OpenMP parallelism"
for i in 128 64 32
do
export OMP_NUM_THREADS=$i
echo "CPUS: " $OMP_NUM_THREADS
./gof 0 > gof_$i.out
done
echo "DONE "