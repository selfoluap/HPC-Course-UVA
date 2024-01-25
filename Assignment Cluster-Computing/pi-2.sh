#!/bin/bash
#SBATCH --job-name="pi"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --time=00:10:00
#SBATCH --partition=thin_course
#SBATCH --output=pi-2_%j.out
#SBATCH --error=pi-2_%j.err
module purge
module load 2023
module load GCC/12.3.0

echo "OpenMP parallelism"
echo
export OMP_NUM_THREADS=32
for i in 31250000 62500000 125000000 250000000 500000000 1000000000 2000000000
do
    ./pi $i
    echo "DONE with $i"
done