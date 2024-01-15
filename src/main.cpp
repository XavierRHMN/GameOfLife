#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "GameOfLife.h"
#include <SDL.h>
#include <iostream>
#include <fstream>

// Constants for window dimensions and control panel sizes
const int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE;
const int CONTROL_PANEL_HEIGHT = WINDOW_HEIGHT;
const int CONTROL_PANEL_WIDTH = 500;
const int HELP_WINDOW_WIDTH = 400;
const int HELP_WINDOW_HEIGHT = 400;

// Simulation state variables
bool isPaused = false; // Controls the simulation state
bool running = true; // Controls the main loop
Uint32 lastUpdateTime = 0; // Stores the last update time
int updateInterval = 100; // Time in milliseconds between updates
int cursorSize = 1;  // Initial cursor size
bool isWindowSizeSet = false; // Flag to set the window size

// Mouse interaction variables
int mouseX, mouseY;
bool leftMouseButtonPressed = false;
bool rightMouseButtonPressed = false; 
bool simulationJustResumed = false;
int hoverX = -1, hoverY = -1;  // Hover cell coordinates
bool showHelpWindow = false; // Flag to show help window

// Cell colours and state;
bool ALIVE = true;
ImVec4 aliveColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
ImVec4 deadColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
ImVec4 gridColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White

SDL_Renderer* tempRenderer = nullptr;

// Main function
int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create SDL window
    SDL_Window* window_ptr = SDL_CreateWindow(
        "Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH + CONTROL_PANEL_WIDTH,
        WINDOW_HEIGHT, 
        SDL_WINDOW_SHOWN);

    if (!window_ptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create SDL renderer
    SDL_Renderer* renderer_ptr = SDL_CreateRenderer(window_ptr, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_ptr) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window_ptr);
        SDL_Quit();
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForSDLRenderer(window_ptr, renderer_ptr);
    ImGui_ImplSDLRenderer2_Init(renderer_ptr);

    // Initialize Game of Life grid
    initializeGrid();

    // Main loop
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_MOUSEMOTION) {
                hoverX = e.motion.x / CELL_SIZE;
                hoverY = e.motion.y / CELL_SIZE;
            }

            if (e.type == SDL_QUIT) {
                running = false;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {
                rightMouseButtonPressed = true;
            }

            // Check for right mouse button up
            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT) {
                rightMouseButtonPressed = false;
            }

            // Check for mouse button down
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                leftMouseButtonPressed = true;
            }

            // Check for mouse button up
            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                leftMouseButtonPressed = false;
            }

            if (rightMouseButtonPressed && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;

                int gridX = mouseX / CELL_SIZE;
                int gridY = mouseY / CELL_SIZE;

            // Set cells within the cursor size to dead
                for (int dx = 0; dx < cursorSize; ++dx) {
                    for (int dy = 0; dy < cursorSize; ++dy) {
                        int cellX = gridX + dx;
                        int cellY = gridY + dy;
                        if (cellX < GRID_WIDTH && cellY < GRID_HEIGHT) {
                            setCellState(cellX, cellY, false); // Set the cell to dead
                        }
                    }
                }
            }

            // Handle mouse motion
            if (leftMouseButtonPressed && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;

                int gridX = mouseX / CELL_SIZE;
                int gridY = mouseY / CELL_SIZE;

                // Set cells within the cursor size to alive
                for (int dx = 0; dx < cursorSize; ++dx) {
                    for (int dy = 0; dy < cursorSize; ++dy) {
                        int cellX = gridX + dx;
                        int cellY = gridY + dy;
                        if (cellX < GRID_WIDTH && cellY < GRID_HEIGHT) {
                            setCellState(cellX, cellY, true); // Set the cell to alive
                        }
                    }
                }
            }
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastUpdateTime > updateInterval) {

            // Update Game of Life
            if (!isPaused && simulationJustResumed) {
                updateGrid(true);  // Use the temp grid for this update
                simulationJustResumed = false;
            } else if (!isPaused) {
                updateGrid();  // Normal update
            }
        
            lastUpdateTime = currentTime;
        }

         // Clear the screen
        SDL_SetRenderDrawColor(renderer_ptr, 120, 180, 255, 255);
        SDL_RenderClear(renderer_ptr);

        // Render the Game of Life grid
        renderGrid(renderer_ptr);

        // Start ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window_ptr);
        ImGui::NewFrame();

        // Render ImGui windows and widgets
        renderImGuiWidgets(renderer_ptr);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer_ptr);
    }

    cleanUp(renderer_ptr, window_ptr);
    return 0;  // Ensure to return 0
}

// Function to render the Game of Life grid
void renderGrid(SDL_Renderer* renderer) {
    auto& grid = getGrid(); // Access the grid from GameOfLife

    SDL_RenderClear(renderer);
    
    // Iterate through each cell in the grid
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {


            SDL_Rect cell;
            cell.x = x * CELL_SIZE;
            cell.y = y * CELL_SIZE;
            cell.w = CELL_SIZE;
            cell.h = CELL_SIZE;

            // Set color based on cell state and hover state
            if (x >= hoverX && x < hoverX + cursorSize && y >= hoverY && y < hoverY + cursorSize && grid[x][y] == 0) {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Alive color
            } else if (grid[x][y]) {
                SDL_SetRenderDrawColor(renderer, aliveColor.x * 255, aliveColor.y * 255, aliveColor.z * 255, 255); // Highlight color
            } else {
                SDL_SetRenderDrawColor(renderer, deadColor.x * 255, deadColor.y * 255, deadColor.z * 255, 255); // Dead color
            }

            // Draw cell
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}


int cellSize = CELL_SIZE;
void renderImGuiWidgets(SDL_Renderer* renderer) {
    ImGui::SetNextWindowSize(ImVec2(CONTROL_PANEL_WIDTH, CONTROL_PANEL_HEIGHT)); // replace newWidth and newHeight with the desired values
    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Start/Pause Simulation
    if (ImGui::Button(isPaused ? "Resume Simulation" : "Pause Simulation")) {
        isPaused = !isPaused;
    }

    // Reset the Simulation
    if (ImGui::Button("Reset Simulation")) {
        initializeGrid();
    }

    if (ImGui::Button("Clear Board")) {
        clearGrid();  
    }

    // Save the current state
    if (ImGui::Button("Save State")) {
        // Implement the save functionality
        saveGrid();
    }

    // Load a previously saved state
    if (ImGui::Button("Load State")) {
        // Implement the load functionality
        loadGrid();
    }

    if (ImGui::Button("Help")) {
            showHelpWindow = true;
    }

    if (showHelpWindow) {
        ImGui::SetNextWindowSize(ImVec2(HELP_WINDOW_WIDTH, HELP_WINDOW_HEIGHT)); 
        ImGui::Begin("Help", &showHelpWindow, ImGuiWindowFlags_NoResize);
        ImGui::TextWrapped("The Game of Life, also known simply as Life, is a cellular automaton devised "
                    "by the British mathematician John Horton Conway in 1970. The game is played on an infinite two-dimensional square grid,"
                    "but in our case a finite grid of cells each of which is in one of two possible states, alive or dead. The rules are simple:");
        ImGui::Spacing();
        ImGui::TextWrapped("Birth: A dead cell with exactly three live neighbors becomes a live cell.");
        ImGui::Spacing();
        ImGui::TextWrapped("Survival: A live cell with two or three live neighbors stays alive.");
        ImGui::Spacing();
        ImGui::TextWrapped("Death:");
        ImGui::TextWrapped("   Overpopulation: A live cell with more than three live neighbors dies.");
        ImGui::TextWrapped("   Loneliness: A live cell with fewer than two live neighbors also dies.");
        ImGui::End();
    }

    // Slider for controlling update interval
    ImGui::SliderInt("Update Interval (ms)", &updateInterval, 1000, 1);

    // Slider for cursor size
    ImGui::SliderInt("Cursor Size", &cursorSize, 1, 10);

    // Display current simulation status
    ImGui::Text("Simulation Status: %s", isPaused ? "Paused" : "Running");

    ImGui::ColorEdit3("Alive Cell Color", (float*)&aliveColor);
    ImGui::ColorEdit3("Dead Cell Color", (float*)&deadColor);
    // These colors can be used in the renderGrid function to draw cells

    // Additional custom widgets can be added here

    ImGui::End();
}


void cleanUp(SDL_Renderer* renderer_ptr, SDL_Window* window_ptr) {
    // Destroy the renderer
    if (renderer_ptr != nullptr) {
        SDL_DestroyRenderer(renderer_ptr);
    }

    // Destroy the window
    if (window_ptr != nullptr) {
        SDL_DestroyWindow(window_ptr);
    }

    // Shutdown ImGui
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Quit SDL subsystems
    SDL_Quit();
}


