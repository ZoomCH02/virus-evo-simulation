#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "virus_sim.h"
#include "simulation/core.h"
#include "rendering/render.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) return 1;

    // Инициализация окон и рендереров
    SDL_Window* window = SDL_CreateWindow("ViruSim FX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) return 1;
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 1;
    
    SDL_Window* stats_window = SDL_CreateWindow("Statistics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            350, 400, 0);
    if (!stats_window) return 1;
    
    SDL_Renderer* stats_renderer = SDL_CreateRenderer(stats_window, -1, SDL_RENDERER_ACCELERATED);
    if (!stats_renderer) return 1;
    
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!font) return 1;

    init_simulation();

    // Основной цикл
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                reset_simulation();
            }
        }

        render_simulation(renderer, stats_renderer, font);
        SDL_Delay(16);
    }

    cleanup_simulation();
    
    // Очистка SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(stats_renderer);
    SDL_DestroyWindow(stats_window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}