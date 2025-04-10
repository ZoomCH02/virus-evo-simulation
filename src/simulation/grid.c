#include "grid.h"
#include "../rendering/render.h"
#include <stdlib.h>

void init_grid() {
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = (Cell){HEALTHY, 0, 0};

    grid[rand() % GRID_HEIGHT][rand() % GRID_WIDTH / 2] = (Cell){INFECTED, 0, 0};
}

void count_stats(int* healthy, int* infected, int* dead) {
    *healthy = *infected = *dead = 0;
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++) {
            switch (grid[y][x].state) {
                case HEALTHY: (*healthy)++; break;
                case INFECTED: (*infected)++; break;
                case DEAD: (*dead)++; break;
            }
        }
}