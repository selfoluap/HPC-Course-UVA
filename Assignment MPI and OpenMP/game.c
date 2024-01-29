#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 3000
#define NUM_ITERATIONS 10

#include "grower.h"
#include "glider.h"
#include "beehive.h"

// Function to initialize the global board
void initializeGlobalBoard(int (*globalBoard)[GRID_SIZE], int rank, int pattern) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            globalBoard[i][j] = 0; // Initialize all cells as dead
        }
    }

    if (rank == 0) {
        switch (pattern)
        {
        case 0:
            // Initialize the pattern in the center of the global board
            for (int i = 0; i < GROWER_HEIGHT; i++) {
                for (int j = 0; j < GROWER_WIDTH; j++) {
                    globalBoard[GRID_SIZE / 2 - GROWER_HEIGHT / 2 + i][GRID_SIZE / 2 - GROWER_WIDTH / 2 + j] = grower[i][j];
                }
            }
            break;
        case 1:
            for (int i = 0; i < BEEHIVE_HEIGHT; i++) {
                for (int j = 0; j < BEEHIVE_WIDTH; j++) {
                    globalBoard[GRID_SIZE / 2 - BEEHIVE_HEIGHT / 2 + i][GRID_SIZE / 2 - BEEHIVE_WIDTH / 2 + j] = beehive[i][j];
                }
            }
            break;
        case 2:
            for (int i = 0; i < GLIDER_HEIGHT; i++) {
                for (int j = 0; j < GLIDER_WIDTH; j++) {
                    globalBoard[GRID_SIZE / 2 - GLIDER_HEIGHT / 2 + i][GRID_SIZE / 2 - GLIDER_WIDTH / 2 + j] = glider[i][j];
                }
            }
            break;
        default:
            for (int i = 0; i < GROWER_HEIGHT; i++) {
                for (int j = 0; j < GROWER_WIDTH; j++) {
                    globalBoard[GRID_SIZE / 2 - GROWER_HEIGHT / 2 + i][GRID_SIZE / 2 - GROWER_WIDTH / 2 + j] = grower[i][j];
                }
            }
            break;
    }

}
}

void initializeLocalBoard(int (*localBoard)[GRID_SIZE], int (*globalBoard)[GRID_SIZE], int rank, int size, int localBoardHeight) {
    int startRow = (GRID_SIZE / size) * rank;
    for (int i = 0; i < localBoardHeight; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            localBoard[i][j] = globalBoard[startRow + i][j];
        }
    }
}

// Function to apply Game of Life rules
void gameOfLifeIteration(int (*currentBoard)[GRID_SIZE], int (*nextBoard)[GRID_SIZE], int localBoardHeight) {
    for (int i = 1; i < localBoardHeight - 1; i++) {
        for (int j = 1; j < GRID_SIZE - 1; j++) {
            int aliveNeighbors = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue;
                    aliveNeighbors += currentBoard[i + di][j + dj];
                }
            }
            nextBoard[i][j] = (aliveNeighbors == 3 || (aliveNeighbors == 2 && currentBoard[i][j])) ? 1 : 0;
        }
    }
}


void synchronizeBoards(int (*localBoard)[GRID_SIZE], int (*globalBoard)[GRID_SIZE], int rank, int size, int localBoardHeight) {
    int startRow = (GRID_SIZE / size) * rank;

    // Allocate memory for the receive buffer based on the localBoardHeight
    int (*recvBuffer)[GRID_SIZE] = malloc(localBoardHeight * sizeof(*recvBuffer));
    if (!recvBuffer) {
        fprintf(stderr, "Error allocating memory for receive buffer\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Using MPI_Sendrecv for efficient communication
    MPI_Sendrecv(localBoard[0], localBoardHeight * GRID_SIZE, MPI_INT, 0, 0,
                 recvBuffer[0], localBoardHeight * GRID_SIZE, MPI_INT, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Updating the global board with received data
    if (rank == 0) {
        for (int i = 0; i < localBoardHeight; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                globalBoard[startRow + i][j] = recvBuffer[i][j];
            }
        }
    }

    // Broadcasting the updated global board from the master process to all other processes
    MPI_Bcast(globalBoard, GRID_SIZE * GRID_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Free the receive buffer
    free(recvBuffer);
}


// Function to count living cells
int countLivingCells(int (*board)[GRID_SIZE]) {
    // Count and return the number of living cells
    int count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            count += board[i][j];
        }
    }
    return count;
}

void printBoard(int (*board)[GRID_SIZE]) {
    // Print the board
    printf("Board:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("Row %d: ", i);
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;

    clock_t start;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        printf("Usage: ./game <pattern>\n");
        printf("Patterns:\n");
        printf("0 - Grower\n");
        printf("1 - Glider\n");
        printf("2 - Beehive\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(1);
    }

    // Calculate the height of each local board based on the number of processes
    int localBoardHeight = GRID_SIZE / size;

    // Allocate memory for the global board
    int (*globalBoard)[GRID_SIZE] = malloc(GRID_SIZE * sizeof(*globalBoard));
    if (!globalBoard) {
        fprintf(stderr, "Error allocating memory for global board\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize the global board with the pattern
    initializeGlobalBoard(globalBoard, rank, atoi(argv[1]));

    // Allocate memory for local boards based on the calculated localBoardHeight
    int (*currentLocalBoard)[GRID_SIZE] = malloc(localBoardHeight * sizeof(*currentLocalBoard));
    int (*nextLocalBoard)[GRID_SIZE] = malloc(localBoardHeight * sizeof(*nextLocalBoard));
    if (!currentLocalBoard || !nextLocalBoard) {
        fprintf(stderr, "Error allocating memory for local boards\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize the local board
    initializeLocalBoard(currentLocalBoard, globalBoard, rank, size, localBoardHeight);

    // Main simulation loop
    for (int iteration = 0; iteration < NUM_ITERATIONS; iteration++) {
        // Apply Game of Life rules to the local board
        gameOfLifeIteration(currentLocalBoard, nextLocalBoard, localBoardHeight);

        // Synchronize the local boards with the global board
        synchronizeBoards(currentLocalBoard, globalBoard, rank, size, localBoardHeight);

        // Swap pointers for next iteration
        int (*temp)[GRID_SIZE] = currentLocalBoard;
        currentLocalBoard = nextLocalBoard;
        nextLocalBoard = temp;
    }

    // If master process, count and print the number of living cells
    if (rank == 0) {
        printBoard(globalBoard);
        printf("Living cells after %d iterations: %d\n", NUM_ITERATIONS, countLivingCells(globalBoard));
        clock_t end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time spent: %f\n", time_spent);
    }

    // Free allocated memory
    free(globalBoard);
    free(currentLocalBoard);
    free(nextLocalBoard);



    // Finalize MPI
    MPI_Finalize();
    return 0;
}
