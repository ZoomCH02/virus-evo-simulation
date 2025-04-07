#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define GRID_WIDTH 100
#define GRID_HEIGHT 100
#define CELL_SIZE 8

#define WINDOW_WIDTH (GRID_WIDTH * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * CELL_SIZE)

#define INFECTION_RADIUS 1
#define INFECTION_PROBABILITY 0.25f
#define RECOVERY_TIME 500
#define DEATH_PROBABILITY 0.1f

typedef enum {
    HEALTHY,
    INFECTED,
    RECOVERED,
    DEAD
} CellState;

typedef struct {
    CellState state;
    int infection_timer;
} Cell;

Cell grid[GRID_HEIGHT][GRID_WIDTH];
Cell next_grid[GRID_HEIGHT][GRID_WIDTH]; // для следующего состояния

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void init_grid() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x].state = HEALTHY;
            grid[y][x].infection_timer = 0;
        }
    }

    // Заразим одну случайную клетку
    int cx = rand() % GRID_WIDTH;
    int cy = rand() % GRID_HEIGHT;
    grid[cy][cx].state = INFECTED;
}

void draw_cell(int x, int y, SDL_Color color) {
    SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void render_grid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // чёрный фон
    SDL_RenderClear(renderer);

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            SDL_Color color;
            switch (grid[y][x].state) {
                case HEALTHY: color = (SDL_Color){0, 255, 0}; break;
                case INFECTED: color = (SDL_Color){255, 0, 0}; break;
                case RECOVERED: color = (SDL_Color){0, 0, 255}; break;
                case DEAD: color = (SDL_Color){50, 50, 50}; break;
            }
            draw_cell(x, y, color);
        }
    }

    SDL_RenderPresent(renderer);
}

bool is_in_bounds(int x, int y) {
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

void update_grid() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell cell = grid[y][x];
            next_grid[y][x] = cell;

            if (cell.state == INFECTED) {
                next_grid[y][x].infection_timer++;

                // Попытка заразить соседей
                for (int dy = -INFECTION_RADIUS; dy <= INFECTION_RADIUS; dy++) {
                    for (int dx = -INFECTION_RADIUS; dx <= INFECTION_RADIUS; dx++) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if ((dx != 0 || dy != 0) && is_in_bounds(nx, ny)) {
                            if (grid[ny][nx].state == HEALTHY) {
                                float chance = (float)rand() / RAND_MAX;
                                if (chance < INFECTION_PROBABILITY) {
                                    next_grid[ny][nx].state = INFECTED;
                                    next_grid[ny][nx].infection_timer = 0;
                                }
                            }
                        }
                    }
                }

                // Проверка: вылечился или умер
                if (cell.infection_timer > RECOVERY_TIME) {
                    float death_chance = (float)rand() / RAND_MAX;
                    next_grid[y][x].state = (death_chance < DEATH_PROBABILITY) ? DEAD : RECOVERED;
                    next_grid[y][x].infection_timer = 0;
                }
            }
        }
    }

    // Копируем next_grid обратно
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = next_grid[y][x];
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("ViruSim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_grid();

    bool running = true;
    SDL_Event event;


    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        update_grid();
        render_grid();
        SDL_Delay(50); // скорость симуляции
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}