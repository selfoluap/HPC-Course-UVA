#include <stdio.h>
#include <stdlib.h>
//#include <mpi.h>

int rank, size;

static int current_grid[3000][3000];
static int new_grid[3000][3000];

void init_grid() {
    for (int i = 0; i < 3000; i++) {
        for (int j = 0; j < 3000; j++) {
            current_grid[i][j] = 0;
        }
    }
}

void update_cell(int i, int j){
    int alive_neighbours = 0;
    int dead_neighbours = 0;

    current_grid[i-1][j-1] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i-1][j] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i-1][j+1] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i][j-1] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i][j+1] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i+1][j-1] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i+1][j] == 1 ? alive_neighbours++ : dead_neighbours++;
    current_grid[i+1][j+1] == 1 ? alive_neighbours++ : dead_neighbours++;

    if (current_grid[i][j] == 1) {
        if (alive_neighbours < 2) {
            new_grid[i][j] = 0;
        }
        else if (alive_neighbours == 2 || alive_neighbours == 3) {
            new_grid[i][j] = 1;
        }
        else if (alive_neighbours > 3) {
            new_grid[i][j] = 0;
        }
    }
    else {
        if (alive_neighbours == 3) {
            new_grid[i][j] = 1;
        }
    }
}



int main(int argc, char** argv) {

/*     MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Hello from process %d of %d\n", rank, size);

    MPI_Finalize(); */

    return 0;
}
