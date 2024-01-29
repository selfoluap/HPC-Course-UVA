#!/bin/bash
#SBATCH --job-name="game_of_life"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --time=00:10:00
#SBATCH --partition=thin_course
./game_of_life