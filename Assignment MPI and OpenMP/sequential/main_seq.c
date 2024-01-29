#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include "glider.h"
#include "beehive.h"
#include "grower.h"

#define WIDTH 3000
#define HEIGHT 3000

// Function prototypes
void initializeBoard(int board[HEIGHT][WIDTH], int pattern);
void printBoard(int board[HEIGHT][WIDTH]);
void updateBoard(int board[HEIGHT][WIDTH]);
int number_living_cells(int board[HEIGHT][WIDTH], int size);

int main(int argc, char *argv[]) {
    int board[HEIGHT][WIDTH];
    initializeBoard(board, 0);
    printBoard(board);

    // Game loop
    for (int iter = 0; iter < 10; iter++) {
        updateBoard(board);
        printBoard(board);
    }

    printf("Number of living cells: %d\n", number_living_cells(board, HEIGHT));
    return 0;
}

void initializeBoard(int board[HEIGHT][WIDTH], int pattern) {
    // Initialize all cells to dead
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board[i][j] = 0;
        }
    }

    // Initialize the grid so that the first element of the pattern is width/2, height/2 of the grid
    int center = WIDTH / 2;
    switch (pattern)
    {
    case 0:
        for (int i = 0; i < GROWER_HEIGHT; i++) {
            for (int j = 0; j < GROWER_WIDTH; j++) {
                int gridX = center + i;
                int gridY = center + j;
                board[gridX][gridY] = grower[i][j];
            }
        }
        break;
    case 1:
        for (int i = 0; i < BEEHIVE_HEIGHT; i++) {
            for (int j = 0; j < BEEHIVE_WIDTH; j++) {
                int gridX = center + i;
                int gridY = center + j;
                board[gridX][gridY] = beehive[i][j];
            }
        }
        break;
    case 2:
        for (int i = 0; i < GLIDER_HEIGHT; i++) {
            for (int j = 0; j < GLIDER_WIDTH; j++) {
                int gridX = center + i;
                int gridY = center + j;
                board[gridX][gridY] = glider[i][j];
            }
        }
        break;
    default:
        break;
    }

}

void printBoard(int board[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c ", board[i][j] ? 'X' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

void updateBoard(int board[HEIGHT][WIDTH]) {
    int newBoard[HEIGHT][WIDTH];

    // Using OpenMP for parallel processing
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int liveNeighbors = 0;

            // Count live neighbors
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (x == 0 && y == 0) continue; // Skip the current cell
                    int ni = i + x, nj = j + y;
                    if (ni >= 0 && ni < HEIGHT && nj >= 0 && nj < WIDTH) {
                        liveNeighbors += board[ni][nj];
                    }
                }
            }

            // Apply the Game of Life rules
            if (board[i][j] == 1 && (liveNeighbors < 2 || liveNeighbors > 3)) {
                newBoard[i][j] = 0; // Cell dies
            } else if (board[i][j] == 0 && liveNeighbors == 3) {
                newBoard[i][j] = 1; // Cell becomes alive
            } else {
                newBoard[i][j] = board[i][j]; // Stays the same
            }
        }
    }

    // Copy newBoard back to board
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board[i][j] = newBoard[i][j];
        }
    }
}


int number_living_cells(int board[HEIGHT][WIDTH], int size) {
    int living_cells = 0;
    #pragma omp parallel for collapse(2) reduction(+:living_cells)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++){
            living_cells += board[i][j];
        }
    }
    return living_cells;
}