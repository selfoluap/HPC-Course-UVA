#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "glider.h"
#include "beehive.h"
#include "grower.h"

#define WIDTH 3000
#define HEIGHT 3000
#define ITERATIONS 5000

void update_board(int **board, int **new_board, int height, int width) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int live_neighbors = 0;

            // Simplified boundary checks
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue;
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        live_neighbors += board[ni][nj];
                    }
                }
            }

            // Game of Life rules
            if (board[i][j] == 1 && (live_neighbors < 2 || live_neighbors > 3)) {
                new_board[i][j] = 0;
            } else if (board[i][j] == 0 && live_neighbors == 3) {
                new_board[i][j] = 1;
            } else {
                new_board[i][j] = board[i][j];
            }
        }
    }

    // Copy the new values back to the original board
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            board[i][j] = new_board[i][j];
        }
    }
}

int count_live_cells(int **board, int height, int width) {
    int count = 0;
    #pragma omp parallel for reduction(+:count)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            count += board[i][j];
        }
    }
    return count;
}

int main(int argc, char *argv[]) {

    //first allocate memory for the board
    int **board = (int **)malloc(HEIGHT * sizeof(int *));
    if (board == NULL) {
        printf("Error allocating memory for the board\n");
        exit(1);
    }

    // Allocation of new_board
    int **new_board = (int **)malloc(HEIGHT * sizeof(int *));
    for (int i = 0; i < HEIGHT; i++) {
        new_board[i] = (int *)malloc(WIDTH * sizeof(int));
    }

    for (int i = 0; i < HEIGHT; i++) {
        board[i] = (int *)malloc(WIDTH * sizeof(int));
        if (board[i] == NULL) {
            printf("Error allocating memory for the board\n");
            exit(1);
        }
    }


    //initialize the board with only dead cells
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board[i][j] = 0;
        }
    }

    //based on user input, initialize the board with a pattern
    if (argc != 2) {
        printf("Usage: ./main <pattern>\n");
        printf("Available patterns: grower (0), beehive (1), glider (2)\n");
        exit(1);
    }

    // Initialize the grid so that the first element of the pattern is width/2, height/2 of the grid
    switch (atoi(argv[1]))
    {
    case 0:
        for (int i = 0; i < GROWER_HEIGHT; i++) {
            for (int j = 0; j < GROWER_WIDTH; j++) {
                int gridX = HEIGHT / 2 + i;
                int gridY = WIDTH / 2 + j;
                board[gridX][gridY] = grower[i][j];
            }
        }
        break;
    case 1:
        for (int i = 0; i < BEEHIVE_HEIGHT; i++) {
            for (int j = 0; j < BEEHIVE_WIDTH; j++) {
                int gridX = HEIGHT / 2 + i;
                int gridY = WIDTH / 2 + j;
                board[gridX][gridY] = beehive[i][j];
            }
        }

        break;
    case 2:
        for (int i = 0; i < GLIDER_HEIGHT; i++) {
            for (int j = 0; j < GLIDER_WIDTH; j++) {
                int gridX = HEIGHT / 2 + i;
                int gridY = WIDTH / 2 + j;
                board[gridX][gridY] = glider[i][j];
            }
        }
        break;
    default:
        break;
    }

    // Measure the time
    double start, end;
    start = omp_get_wtime();

    // Run the simulation
    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        update_board(board, new_board, HEIGHT, WIDTH);
    }

    end = omp_get_wtime();


    printf("Done.\n It took %f seconds\n", end-start);
    printf("The number of living cells is: %d\n", count_live_cells(board, HEIGHT, WIDTH));

    // Free the memory allocated for the board
    for (int i = 0; i < HEIGHT; i++) {
        free(board[i]);
        free(new_board[i]);
    }
    free(board);
    free(new_board);

    return 0;
}
