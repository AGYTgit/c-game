#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int FPS = 240;
const double FRAME_TIME = 1000.0f / FPS;

float calcFPS(float elapsedTime) {
    return 1000.0f / elapsedTime;
}

// Function to draw a filled circle
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("SDL Window Example",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create a renderer for the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


    int rectWidth = 100;
    int rectHeight = 100;

    // define rect
    SDL_Rect rect = { (SCREEN_WIDTH - rectWidth)/2, (SCREEN_HEIGHT - rectHeight)/2, 100, 100 };

    // Initialize performance counters
    Uint64 startTicks = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    int quit = 0;
    SDL_Event e;

    // Track the number of frames and the time for FPS calculation
    int frameCount = 0;
    float fps = 0.0f;

    // Main loop flag
    while (!quit) {
        // Get current time in ticks
        Uint64 currentTicks = SDL_GetPerformanceCounter();

        // Calculate the time elapsed in milliseconds
        float elapsedTime = (float)(currentTicks - startTicks) / (float)frequency * 1000.0f;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                }

                if (e.key.keysym.sym == SDLK_SPACE) {
                    rect.x = (rect.x + 1) % SCREEN_WIDTH;
                }
            }
        }

        if (elapsedTime >= FRAME_TIME) {
            SDL_RenderPresent(renderer);

            SDL_SetRenderDrawColor(renderer, 11, 10, 15, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 200, 240, 255);
            SDL_RenderFillRect(renderer, &rect);
            // DrawCircle(renderer, 200, 200, 100);

            printf("FPS: %.2f, %f\n", calcFPS(elapsedTime), elapsedTime);

            startTicks = SDL_GetPerformanceCounter();
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
