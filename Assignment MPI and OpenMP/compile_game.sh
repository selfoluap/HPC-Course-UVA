#!/bin/bash
module load 2023
module load GCC/12.3.0
module load OpenMPI/4.1.5-GCC-12.3.0
mpicc -o game_of_life game.c
