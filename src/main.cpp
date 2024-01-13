#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "GameOfLife.h"
#include <SDL.h>

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

// Main function
int main(int argc, char* argv[]) {


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }

    SDL_Window* window_ptr = SDL_CreateWindow(
        "Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );

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

    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
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

            if (e.type == SDL_MOUSEMOTION && rightMouseButtonPressed) {
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
            if (e.type == SDL_MOUSEMOTION && leftMouseButtonPressed) {
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

        // ImGui toolbar for control
        
        ImGui::Begin("Control Panel");
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

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            SDL_Rect cell;
            cell.x = x * 10;  // Assuming each cell is 10x10 pixels
            cell.y = y * 10;
            cell.w = 10;
            cell.h = 10;

            if (grid[x][y]) { // Alive cells
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
            } else { // Dead cells
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color
            }
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}
