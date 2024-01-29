#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "glider.h"
#include "beehive.h"
#include "grower.h"
#include <string.h>
#include <time.h>



void initialize_grid(int *grid, int size, int pattern) {
    // Initialize the grid with starting state
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid[i * size + j] = 0;
        }
    }

    switch (pattern)
    {
    case 0:
        // Initialize the pattern in the center of the global board
        for (int i = 0; i < GROWER_HEIGHT; i++) {
            for (int j = 0; j < GROWER_WIDTH; j++) {
                int gridIndex = (size / 2 - GROWER_HEIGHT / 2 + i) * size + (size / 2 - GROWER_WIDTH / 2 + j);
                grid[gridIndex] = grower[i][j];
            }
        }      
        break;
    case 1:
        for (int i = 0; i < BEEHIVE_HEIGHT; i++) {
            for (int j = 0; j < BEEHIVE_WIDTH; j++) {
                int gridIndex = (size / 2 - BEEHIVE_HEIGHT / 2 + i) * size + (size / 2 - BEEHIVE_WIDTH / 2 + j);
                grid[gridIndex] = beehive[i][j];
            }
        }    

        break;
    case 2:
        for (int i = 0; i < GLIDER_HEIGHT; i++) {
            for (int j = 0; j < GLIDER_WIDTH; j++) {
                int gridIndex = (size / 2 - GLIDER_HEIGHT / 2 + i) * size + (size / 2 - GLIDER_WIDTH / 2 + j);
                grid[gridIndex] = glider[i][j];
            }
        }
        break;
    default:
        break;
    }
}

void compute_next_step(int *grid, int size, int world_rank, int world_size) {
    int rows_per_process = size / world_size;
    int *new_grid = (int *)malloc(rows_per_process * size * sizeof(int));

    send_receive_edges(grid, size, world_rank, world_size);

    #pragma omp parallel for
    for (int i = 0; i < rows_per_process; i++) {
        for (int j = 0; j < size; j++) {
            int alive_neighbors = count_alive_neighbors(grid, size, i, j);
            int index = i * size + j;
            if (grid[index] == 1) {
                new_grid[index] = (alive_neighbors == 2 || alive_neighbors == 3) ? 1 : 0;
            } else {
                new_grid[index] = (alive_neighbors == 3) ? 1 : 0;
            }
        }
    }

    memcpy(grid, new_grid, rows_per_process * size * sizeof(int));
    free(new_grid);
}


int count_alive_neighbors(int *grid, int size, int x, int y) {
    int alive = 0;
    int row, col;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Skip the cell itself

            row = (x + i + size) % size; // Wrap around the grid if necessary
            col = (y + j + size) % size;

            if (grid[row * size + col] == 1) {
                alive++;
            }
        }
    }

    return alive;
}

void distribute_grid(int *grid, int grid_size, int world_rank, int number_of_processes) {
    int rows_per_process = grid_size / number_of_processes;
    int *subgrid = (int *)malloc(rows_per_process * grid_size * sizeof(int));

    // Scatter sends chunks of an array to each process
    MPI_Scatter(grid, rows_per_process * grid_size, MPI_INT, 
                subgrid, rows_per_process * grid_size, MPI_INT, 
                0, MPI_COMM_WORLD);

    // Replace the original grid with the subgrid in each process
    memcpy(grid, subgrid, rows_per_process * grid_size * sizeof(int));
    free(subgrid);
}

void gather_grid(int *grid, int size, int world_rank, int world_size) {
    int rows_per_process = size / world_size;
    int *complete_grid = NULL;

    if (world_rank == 0) {
        complete_grid = (int *)malloc(size * size * sizeof(int));
    }

    MPI_Gather(grid, rows_per_process * size, MPI_INT, 
               complete_grid, rows_per_process * size, MPI_INT, 
               0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        memcpy(grid, complete_grid, size * size * sizeof(int));
        free(complete_grid);
    }
}


void send_receive_edges(int *grid, int size, int world_rank, int world_size) {
    MPI_Status status;
    int rows_per_process = size / world_size;
    int *top_row = (int *)malloc(size * sizeof(int));
    int *bottom_row = (int *)malloc(size * sizeof(int));

    if (world_rank > 0) {
        MPI_Sendrecv(grid, size, MPI_INT, world_rank - 1, 0,
                     bottom_row, size, MPI_INT, world_rank + 1, 0,
                     MPI_COMM_WORLD, &status);
    }

    if (world_rank < world_size - 1) {
        MPI_Sendrecv(&grid[(rows_per_process - 1) * size], size, MPI_INT, world_rank + 1, 0,
                     top_row, size, MPI_INT, world_rank - 1, 0,
                     MPI_COMM_WORLD, &status);
    }

    if (world_rank > 0) {
        memcpy(grid - size, bottom_row, size * sizeof(int)); // Insert the bottom row at the top
    }
    if (world_rank < world_size - 1) {
        memcpy(grid + rows_per_process * size, top_row, size * sizeof(int)); // Insert the top row at the bottom
    }

    free(top_row);
    free(bottom_row);
}

void print_grid(int *grid, int size) {
    // just a helper function to print the grid
    printf("Grid:\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", grid[i * size + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int count_living_cells(int *grid, int size) {
    // just a helper function to count the living cells
    int count = 0;
    for (int i = 0; i < size * size; i++) {
        if (grid[i] == 1)
            count++;
    }
    return count;
}


int main(int argc, char *argv[]) {
    clock_t start = clock();
    int world_size, world_rank;

    // basic MPI initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // check for correct usage
    if (argc != 4) {
        if (world_rank == 0) {
            printf("Usage: %s <grid_size> <number_of_iterations> <pattern>\n", argv[0]);
            printf("Patterns:\n");
            printf("0: Grower\n");
            printf("1: Beehive\n");
            printf("2: Glider\n");
        }
        MPI_Finalize();
        return 0;
    }

   
    int grid_size = atoi(argv[1]);  // Example size, adjust as needed
    int *grid = (int*)malloc(grid_size * grid_size * sizeof(int));

    if (world_rank == 0) {
        // Initialize the grid with starting state
        initialize_grid(grid, grid_size, atoi(argv[3]));
    }

    // Distribute the grid to all processes
    distribute_grid(grid, grid_size, world_rank, world_size);

    
    for (int step = 0; step < atoi(argv[2]); step++) { 
        compute_next_step(grid, grid_size, world_rank, world_size);

        gather_grid(grid, grid_size, world_rank, world_size);

        if (world_rank == 0) {
            print_grid(grid, grid_size);
            printf("Step %d: %d living cells\n", step, count_living_cells(grid, grid_size));
        }
    }

    if (world_rank == 0) {
        printf("Total time: %f\n", (double)(clock() - start) / CLOCKS_PER_SEC);
    }

    free(grid);
    MPI_Finalize();
    return 0;
}
