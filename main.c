#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

const int BG_R = 11;
const int BG_G = 10;
const int BG_B = 15;
const int BG_O = 255;

typedef struct {
    float x, y, w, h;
    float vx, vy;
    int r, g, b;
    float mass;
} Square;
Square* s;
int sLen = 0;
const int sMaxLen = 2 << 4;

void addSqr(float x, float y, float w, float h, float vx, float vy, float mass) {
    s[sLen++] = (Square){x, y, w, h, vx, vy, rand() % 256, rand() % 256, rand() % 256, mass};
}

int checkAABBCollision(const Square* r1, const Square* r2) {
    float dx = fabs(r1->x - r2->x);
    float dy = fabs(r1->y - r2->y);
    float combinedWidth = (r1->w + r2->w) / 2.0f;
    float combinedHeight = (r1->h + r2->h) / 2.0f;

    return dx <= combinedWidth && dy <= combinedHeight;
}

void resolveAABBCollision(Square* r1, Square* r2) {
    float dx = r2->x - r1->x;
    float dy = r2->y - r1->y;
    float combinedWidth = (r1->w + r2->w) / 2.0f;
    float combinedHeight = (r1->h + r2->h) / 2.0f;

    float overlapX = combinedWidth - fabs(dx);
    float overlapY = combinedHeight - fabs(dy);

    if (overlapX < overlapY) {
        if (dx > 0) {
            r1->x -= overlapX / 2.0f;
            r2->x += overlapX / 2.0f;
        } else {
            r1->x -= overlapX * (r2->mass / (r1->mass + r2->mass));
            r2->x += overlapX * (r1->mass / (r1->mass + r2->mass));
        }

        float tempVx1 = r1->vx;
        r1->vx = ((r1->mass - r2->mass) * r1->vx + 2 * r2->mass * r2->vx) / (r1->mass + r2->mass);
        r2->vx = ((r2->mass - r1->mass) * r2->vx + 2 * r1->mass * tempVx1) / (r1->mass + r2->mass);
    } else {
        if (dy > 0) {
            r1->y -= overlapY / 2.0f;
            r2->y += overlapY / 2.0f;
        } else {
            r1->y += overlapY / 2.0f;
            r2->y -= overlapY / 2.0f;
        }

        float tempVy1 = r1->vy;
        r1->vy = ((r1->mass - r2->mass) * r1->vy + 2 * r2->mass * r2->vy) / (r1->mass + r2->mass);
        r2->vy = ((r2->mass - r1->mass) * r2->vy + 2 * r1->mass * tempVy1) / (r1->mass + r2->mass);
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    s = (Square*)malloc(sMaxLen * sizeof(Square));

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

    addSqr(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 100, 100, 1, 1, 1000);

    while (!quit) {
        float loopStartTime = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                quit = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_SPACE) {
                addSqr(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT / 2 - 25, 50, 50, (rand() % 1000) / 1000.0 - 0.5, (rand() % 1000) / 1000.0 - 0.5, 1);
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
            }
        }

        for (int i = 0; i < sLen; i++) {
            s[i].x += s[i].vx;
            s[i].y += s[i].vy;
        }

        for (int i = 0; i < sLen; i++) {
            if ((s[i].x <= 0 && s[i].vx < 0) || (s[i].x >= (SCREEN_WIDTH - s[i].w) && s[i].vx > 0)) {
                s[i].vx *= -1;
            }
            if ((s[i].y <= 0 && s[i].vy < 0) || s[i].y >= (SCREEN_HEIGHT - s[i].h) && s[i].vy > 0) {
                s[i].vy *= -1;
            }
        }

        for (int i = 0; i < sLen; ++i) {
            for (int j = i + 1; j < sLen; ++j) {
                if (checkAABBCollision(&s[i], &s[j])) {
                    resolveAABBCollision(&s[i], &s[j]);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, BG_R, BG_G, BG_B, BG_O);
        SDL_RenderClear(renderer);

        for (int i = 0; i < sLen; i++) {
            SDL_SetRenderDrawColor(renderer, s[i].r, s[i].g, s[i].b, 255);
            SDL_FRect rect = (SDL_FRect){s[i].x, s[i].y, s[i].w, s[i].h};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(s);

    return 0;
}
