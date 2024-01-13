#include "GameOfLife.h"
#include <random>

// Grid representation
std::vector<std::vector<bool>> grid(GRID_WIDTH, std::vector<bool>(GRID_HEIGHT, false));
std::vector<std::vector<bool>> tempGrid(GRID_WIDTH, std::vector<bool>(GRID_HEIGHT, false));

// Initialize the grid with random values
void initializeGrid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution distribution(0.5); // 50% chance for each cell

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            grid[x][y] = distribution(gen); // Randomly alive or dead
        }
    }
}

void toggleCell(int x, int y) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        grid[x][y] = !grid[x][y];
    }
}

// Count the live neighbors of a cell
int countLiveNeighbors(const std::vector<std::vector<bool>>& sourceGrid, int x, int y) {
    int liveNeighbors = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Skip the cell itself

            int newX = x + i;
            int newY = y + j;

            // Check for boundary conditions
            if (newX >= 0 && newX < GRID_WIDTH && newY >= 0 && newY < GRID_HEIGHT) {
                liveNeighbors += sourceGrid[newX][newY] ? 1 : 0;
            }
        }
    }

   
return liveNeighbors;
}

void copyGrid() {
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            tempGrid[x][y] = grid[x][y];
        }
    }
}

// Update the grid based on Game of Life rules
void updateGrid(bool useTemp) {
    auto& sourceGrid = useTemp ? tempGrid : grid;
    std::vector<std::vector<bool>> newGrid(GRID_WIDTH, std::vector<bool>(GRID_HEIGHT, false));

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            int liveNeighbors = countLiveNeighbors(sourceGrid, x, y);

            if (sourceGrid[x][y]) {
                // Cell is currently alive
                newGrid[x][y] = (liveNeighbors == 2 || liveNeighbors == 3);
            } else {
                // Cell is currently dead
                newGrid[x][y] = (liveNeighbors == 3);
            }
        }
    }

    grid = newGrid;
}

void clearGrid() {
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            grid[x][y] = false;  // Set each cell to dead
        }
    }
}

bool getCellState(int x, int y) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        return grid[x][y];
    }
    return false; // Return false for out-of-bounds
}

void setCellState(int x, int y, bool state) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        grid[x][y] = state;
    }
}

std::vector<std::vector<bool>>& getGrid() {
    return grid;
}
