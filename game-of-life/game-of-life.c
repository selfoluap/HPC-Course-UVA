#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>


int count_neighbors(int* grid, int n, int i) {
    int count = 0;
    if (i > 0) {
        count += board[i - 1];
    }
    if (i < n - 1) {
        count += board[i + 1];
    }
    return count;
}



int main(int argc, char** argv) {
    int rank, size;
    int max_iter = 5000;

    int n = 3000;
    int cols = n / sqrt(size);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // TODO: Add your MPI code here
    if (rank == 0) {
        printf("Hello from rank 0\n");
    } else {
        printf("Hello from rank %d\n", rank);
    }

    printf("Number of processes: %d\n", size);

    MPI_Finalize();
    return 0;
}
