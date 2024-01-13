
# Game of Life

## Description
This project is an implementation of Conway's Game of Life, a cellular automaton devised by the British mathematician John Horton Conway in 1970. The game is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. Users can create an initial configuration and observe how it evolves.

## Features
- Interactive grid to draw and erase cells
- Control over the simulation speed
- Pause/resume functionality
- Clear the grid at any time

## Installation
To run this project you will need to enter the following commands into command prompt or another terminal. 

### Steps:
1. Clone the repository:
   ```
   git clone https://github.com/XavierRHMN/GameOfLife.git
   ```
2. Navigate to the project directory:
   ```
   cd GameOfLife
   ```
3. Compile the project (ensure you have g++ and the necessary libraries installed):
   ```
   g++ -o gameoflife src/*.cpp -Iinclude -Llib -lSDL2 -lSDL2main -lImGui 
   ```
4. Run the compiled application:
   ```
   ./gameoflife
   ```

## Usage
After running the application, you will see a grid representing the Game of Life world. 

- Left-click and drag to add cells.
- Right-click and drag to remove cells.
- Use the control panel to pause, resume, clear the grid, or change the simulation speed.

## License
[MIT License](LICENSE)
