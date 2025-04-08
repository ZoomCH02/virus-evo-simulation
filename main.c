#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "virus_sim.h"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("ViruSim FX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Window* stats_window = SDL_CreateWindow("Statistics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            300, 200, 0);
    SDL_Renderer* stats_renderer = SDL_CreateRenderer(stats_window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!font) return 1;

    init_strain();
    init_grid();

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                init_strain();  // <-- добавь это!
                init_grid();
            }
        }

        update_grid();
        render_grid(renderer, font);
        render_stats_window(stats_renderer, font);

        SDL_Delay(50);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(stats_renderer);
    SDL_DestroyWindow(stats_window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}