#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "GameOfLife.h"
#include <SDL.h>
#include <iostream>
#include <fstream>

const int windowWidth = GRID_WIDTH * CELL_SIZE;
const int windowHeight = GRID_HEIGHT * CELL_SIZE;
bool isPaused = false;  // Variable to control the simulation state
bool running = true;
bool isWindowSizeSet = false;
Uint32 lastUpdateTime = 0; // Store the last update time
int updateInterval = 100; // Time in milliseconds between
int cursorSize = 1;  // Start with a single cell cursor
bool isClickedCellAlive = false;
int mouseX, mouseY;
bool leftMouseButtonPressed = false;
bool rightMouseButtonPressed = false; 
bool simulationJustResumed = false;
int hoverX = -1, hoverY = -1;  // Variables to store the hover cell coordinates
const int CONTROL_PANEL_WIDTH = 300;

// Main function
int main(int argc, char* argv[]) {


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }

    SDL_Window* window_ptr = SDL_CreateWindow(
        "Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GRID_WIDTH * CELL_SIZE + CONTROL_PANEL_WIDTH,
        GRID_HEIGHT * CELL_SIZE, 
        SDL_WINDOW_SHOWN);

    
    if (!window_ptr) {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer_ptr = SDL_CreateRenderer(window_ptr, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer_ptr) {
        SDL_DestroyWindow(window_ptr);
        SDL_Quit();
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForSDLRenderer(window_ptr, renderer_ptr);
    ImGui_ImplSDLRenderer2_Init(renderer_ptr);

    // Initialize Game of Life
    initializeGrid();

    int hoverX = -1, hoverY = -1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            
            if (e.type == SDL_MOUSEMOTION) {
                std::cout << "Hovering over: " << hoverX << ", " << hoverY << std::endl;
                hoverX = e.motion.x / CELL_SIZE;
                hoverY = e.motion.y / CELL_SIZE;

                // Optional: Check if the coordinates are within the grid bounds
                if (hoverX >= GRID_WIDTH || hoverY >= GRID_HEIGHT) {
                    hoverX = -1;
                    hoverY = -1;
                }
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

        // Render Game of Life
        SDL_SetRenderDrawColor(renderer_ptr, 120, 180, 255, 255);
        SDL_RenderClear(renderer_ptr);
        renderGrid(renderer_ptr);

        // Render ImGui
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window_ptr);
        ImGui::NewFrame();

        // ImGui control panel
        ImGui::SetNextWindowPos(ImVec2(GRID_WIDTH * CELL_SIZE, 0)); // Set the position of the window
        ImGui::SetNextWindowSize(ImVec2(CONTROL_PANEL_WIDTH, GRID_HEIGHT * CELL_SIZE)); // Set the size of the window
        
        ImGui::Begin("Control Panel", NULL, ImGuiWindowFlags_NoResize);
        ImGui::Text("Game of Life Simulation");

        if (ImGui::Button("Clear Board")) {
            clearGrid();  // Clear the grid when the button is clicked
        }
        if (ImGui::Button("Restart Simulation")) {
            initializeGrid();  // Restart the Game of Life simulation
        }
        if (ImGui::Button(isPaused ? "Resume Simulation" : "Pause Simulation")) {
            isPaused = !isPaused;
        }
        if (!isWindowSizeSet) {
            ImGui::SetNextWindowSize(ImVec2(400, 400)); // Desired initial size
            isWindowSizeSet = true;
        }

        if (ImGui::Button("Save Grid")) {
            saveGrid();
        }

        if (ImGui::Button("Load Grid")) {
            loadGrid();
        }

        ImGui::SliderInt("Cursor Size", &cursorSize, 1, 10);
        ImGui::SliderInt("Speed (ms)", &updateInterval, 1000, 1);
        
        // Add more controls as needed
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer_ptr);
    }

    // Clean up
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    SDL_Quit();

    return 0;  // Ensure to return 0
}

// Function to render the Game of Life grid
void renderGrid(SDL_Renderer* renderer) {
    auto& grid = getGrid();  // Access the grid from GameOfLife

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Convert the mouse position to grid coordinates
    int hoverX = mouseX / CELL_SIZE;
    int hoverY = mouseY / CELL_SIZE;

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            SDL_Rect cell;
            cell.x = x * CELL_SIZE;
            cell.y = y * CELL_SIZE;
            cell.w = CELL_SIZE;
            cell.h = CELL_SIZE;

            // Set color based on cell state and hover state
            if (x >= hoverX && x < hoverX + cursorSize && y >= hoverY && y < hoverY + cursorSize && grid[x][y] == 0) {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Highlight color
            } else if (grid[x][y]) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Alive color
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Dead color
            }

            SDL_RenderFillRect(renderer, &cell);
        }
    }
}
