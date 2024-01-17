
# Game of Life

## Description
This project is an implementation of Conway's Game of Life, a cellular automaton devised by the British mathematician John Horton Conway in 1970. The game is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. Users can create an initial configuration and observe how it evolves.

## Features
- Create and destroy your own cell colonies using left click and right click for drawing and deleting
- Pause or resume simulation or clear grid 
- Change simulation speed, change cell cursor size, change  cell colours
- Save and load grid
- Window rendering using SDL2 and control panel rendering was built with ImGui

## Installation
Navigate to the build folder and run project.exe or if you want to compile and build this project yourself you can do the following (Git and Make need to be installed);

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
   make
   ```
4. Navigate to the build directory:
   ```
   cd build
   ```
5. Run the compiled application:
   ```
   ./project
   ```

## Usage
After running the application, you will see a grid representing the Game of Life world. 

- Left-click and drag to add cells.
- Right-click and drag to remove cells.
- Use the control panel to pause, resume, clear the grid, or change the simulation speed.

## License
[MIT License](LICENSE)
