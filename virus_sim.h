#ifndef VIRUS_SIM_H
#define VIRUS_SIM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>

#define GRID_WIDTH 150
#define GRID_HEIGHT 100
#define CELL_SIZE 8
#define WINDOW_WIDTH (GRID_WIDTH * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * CELL_SIZE)
#define MAX_STRAINS 10
#define MAX_MUTATION_HISTORY 10

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
    float revival_rate;
    int recovery_time;
    Color color;
    int has_mutated;
    int infected_count;
} VirusStrain;

typedef struct {
    CellState state;
    int infection_timer;
    int strain_id;
} Cell;

typedef struct {
    char message[256];
    int strain_id;
} MutationRecord;

extern pthread_mutex_t grid_mutex;
extern pthread_mutex_t strains_mutex;
extern int simulation_running;
extern int rendering_complete;

extern VirusStrain strains[MAX_STRAINS];
extern int strain_count;
extern Cell grid[GRID_HEIGHT][GRID_WIDTH];
extern MutationRecord mutation_history[MAX_MUTATION_HISTORY];
extern int mutation_history_count;

void init_strain();
void init_grid();
void mutate();
void update_grid();
void mutate_from_strain(int parent_id);
void render_grid(SDL_Renderer* renderer, TTF_Font* font);
void count_stats(int* healthy, int* infected, int* dead);
void render_text(const char* text, int x, int y, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font);
void render_stats_window(SDL_Renderer* renderer, TTF_Font* font);
Color get_distinct_color(int index);
void* simulation_thread(void* arg);

#endif