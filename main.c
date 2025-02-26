#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

const int BG_R = 11;
const int BG_G = 10;
const int BG_B = 15;
const int BG_O = 255;


typedef struct square {
    float x;
    float y;
    float w;
    float h;

    float vx;
    float vy;

    int r;
    int g;
    int b;
} Square;
Square* sPtr;
int sPtrLen = 0;
const int sPtrMaxLen = 2<<6;


void addSqr(float x, float y, float w, float h, float vx, float vy) {
    sPtr[sPtrLen++] = (Square){x, y, w, h, vx, vy, rand() % 256, rand() % 256, rand() % 256};
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    sPtr = (Square*)malloc(sPtrMaxLen * sizeof(Square));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 Window", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    int quit = 0;

    addSqr(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT / 2 - 25, 50, 50, 1, 1);

    while (!quit) {
        float loopStartTime = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                quit = 1;
            } else if (event.type = SDL_EVENT_WINDOW_RESIZED) {
                SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
            }
        }

        for (int i = 0; i < sPtrLen; i++) {
            if (sPtr[i].x <= 0) {
                sPtr[i].vx = abs(sPtr[i].vx);
                sPtr[i].x++;
                if (sPtrLen <= sPtrMaxLen) {
                    addSqr(sPtr[i].x, sPtr[i].y, sPtr[i].w, sPtr[i].h, sPtr[i].vx, sPtr[i].vy * -1);
                }
            } else if (sPtr[i].x >= (SCREEN_WIDTH - sPtr[i].w)) {
                sPtr[i].vx = abs(sPtr[i].vx) * -1;
                sPtr[i].x--;
                if (sPtrLen <= sPtrMaxLen) {
                    addSqr(sPtr[i].x, sPtr[i].y, sPtr[i].w, sPtr[i].h, sPtr[i].vx, sPtr[i].vy * -1);
                }
            }
            if (sPtr[i].y <= 0) {
                sPtr[i].vy = abs(sPtr[i].vy);
                sPtr[i].y++;
                if (sPtrLen <= sPtrMaxLen) {
                    addSqr(sPtr[i].x, sPtr[i].y, sPtr[i].w, sPtr[i].h, sPtr[i].vx * -1, sPtr[i].vy);
                }
            } else if (sPtr[i].y >= (SCREEN_HEIGHT - sPtr[i].h)) {
                sPtr[i].vy = abs(sPtr[i].vy) * -1;
                sPtr[i].y--;
                if (sPtrLen <= sPtrMaxLen) {
                    addSqr(sPtr[i].x, sPtr[i].y, sPtr[i].w, sPtr[i].h, sPtr[i].vx * -1, sPtr[i].vy);
                }
            }
        }

        for (int i = 0; i < sPtrLen; i++) {
            sPtr[i].x += sPtr[i].vx;
            sPtr[i].y += sPtr[i].vy;
        }

        SDL_SetRenderDrawColor(renderer, BG_R, BG_G, BG_B, BG_O);
        SDL_RenderClear(renderer);

        for (int i = 0; i < sPtrLen; i++) {
            SDL_SetRenderDrawColor(renderer, sPtr[i].r, sPtr[i].g, sPtr[i].b, 255);
            SDL_FRect rect = (SDL_FRect){sPtr[i].x, sPtr[i].y ,sPtr[i].w ,sPtr[i].h};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(sPtr);

    return 0;
}
