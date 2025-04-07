#ifndef VIRUS_SIM_H
#define VIRUS_SIM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define GRID_WIDTH 100
#define GRID_HEIGHT 100
#define CELL_SIZE 8
#define WINDOW_WIDTH (GRID_WIDTH * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * CELL_SIZE)
#define MAX_STRAINS 64

typedef enum {
    HEALTHY,
    INFECTED,
    DEAD
} CellState;

typedef struct {
    float r, g, b;
} Color;

typedef struct {
    char name[64];
    float infection_rate;
    float death_rate;
    int recovery_time;
    Color color;
} VirusStrain;

typedef struct {
    CellState state;
    int infection_timer;
    int strain_id;
} Cell;

extern VirusStrain strains[MAX_STRAINS];
extern int strain_count;
extern Cell grid[GRID_HEIGHT][GRID_WIDTH];

void init_strain();
void init_grid();
void mutate();
void update_grid();
void render_grid(SDL_Renderer* renderer, TTF_Font* font);
void count_stats(int* healthy, int* infected, int* dead);
void render_text(const char* text, int x, int y, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font);
void render_stats_window(SDL_Renderer* renderer, TTF_Font* font);
Color random_color();

#endif