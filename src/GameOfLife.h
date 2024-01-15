#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <vector>
#include <SDL.h>

// ... other includes and definitions
const int GRID_WIDTH = 50;
const int GRID_HEIGHT = 50;
const int CELL_SIZE = 10;  // Size of each cell in pixels
extern const int windowWidth;
extern const int windowHeight;


std::vector<std::vector<bool>>& getGrid();


// Functions declarations
void initializeGrid();
void updateGrid(bool useTemp = false);
void renderGrid(SDL_Renderer* renderer);
void renderImGuiWidgets(SDL_Renderer* renderer);
void cleanUp(SDL_Renderer* renderer, SDL_Window* window);
void toggleCell(int x, int y);
bool getCellState(int x, int y);
void setCellState(int x, int y, bool state);
void copyGrid();
void clearGrid();
void getTempGrid();
void saveGrid();;
void loadGrid();

#endif // GAME_OF_LIFE_H
