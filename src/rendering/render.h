#ifndef RENDER_H
#define RENDER_H

#include "../../include/virus_sim.h"

void render_simulation(SDL_Renderer* renderer, SDL_Renderer* stats_renderer, TTF_Font* font);
void render_text(const char* text, int x, int y, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font);
void render_stats_window(SDL_Renderer* renderer, TTF_Font* font);

#endif