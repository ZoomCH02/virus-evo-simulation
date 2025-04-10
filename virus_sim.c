#include "virus_sim.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int strain_count = 1;
VirusStrain strains[MAX_STRAINS];
Cell grid[GRID_HEIGHT][GRID_WIDTH];
int ticks_since_last_mutation = 0;
int mutation_timer = 500;
int mutation_fx_counter = 0;
int mutation_cooldown = 0;

Color make_color(float r, float g, float b) {
    Color c = {r, g, b};
    return c;
}

Color get_distinct_color(int index) {
    Color palette[] = {
        {1.0f, 0.0f, 0.0f}, // Красный
        {0.0f, 1.0f, 0.0f}, // Зелёный
        {0.0f, 0.0f, 1.0f}, // Синий
        {1.0f, 1.0f, 0.0f}, // Жёлтый
        {1.0f, 0.0f, 1.0f}, // Пурпурный
        {0.0f, 1.0f, 1.0f}, // Голубой
        {1.0f, 0.5f, 0.0f}, // Оранжевый
        {0.6f, 0.0f, 1.0f}, // Фиолетовый
        {0.3f, 1.0f, 0.2f}, // Лайм
        {0.9f, 0.2f, 0.4f}  // Малиновый
    };
    int size = sizeof(palette) / sizeof(palette[0]);
    return palette[index % size];
}

void init_strain() {
    strain_count = 1;
    for (int i = 0; i < MAX_STRAINS; i++) {
        strains[i] = (VirusStrain){0};  // обнуляем структуру
    }
    strains[0] = (VirusStrain){
        .name = "VRS-0",
        .infection_rate = 0.05f,
        .death_rate = 0.005f,
        .recovery_time = 400,
        .color = make_color(255, 0, 0),
        .has_mutated = 0,
        .infected_count = 0
    };
}

void mutate() {
    if (strain_count >= MAX_STRAINS) return;

    mutation_cooldown = 100;

    VirusStrain* prev = &strains[strain_count - 1];
    VirusStrain* new_strain = &strains[strain_count++];

    snprintf(new_strain->name, sizeof(new_strain->name), "VRS-%d", strain_count - 1);
    new_strain->infection_rate = fminf(fmaxf(prev->infection_rate + ((rand() % 200 - 100) / 1000.0f), 0.01f), 0.9f);
    new_strain->death_rate = fminf(fmaxf(prev->death_rate + ((rand() % 100 - 50) / 1000.0f), 0.01f), 0.5f);
    new_strain->recovery_time = prev->recovery_time + (rand() % 41 - 20);
    if (new_strain->recovery_time < 100) new_strain->recovery_time = 100;
    new_strain->color = get_distinct_color(strain_count - 1);

    mutation_fx_counter = 15;
    printf("[MUTATION] New strain: %s | Infection: %.2f | Death: %.2f | Recovery: %d\n",
           new_strain->name, new_strain->infection_rate, new_strain->death_rate, new_strain->recovery_time);
}

void init_grid() {
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = (Cell){HEALTHY, 0, 0};

    grid[rand() % GRID_HEIGHT][rand() % GRID_WIDTH / 2] = (Cell){INFECTED, 0, 0};
}

void update_grid() {
    // Сначала обновляем все состояния в ОДИН проход
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell cell = grid[y][x];

            if (cell.state == INFECTED) {
                strains[cell.strain_id].infected_count++;

                VirusStrain strain = strains[cell.strain_id];
                grid[y][x].infection_timer++;

                // Проверка на смерть
                if ((rand() / (float)RAND_MAX) < strain.death_rate) {
                    grid[y][x].state = DEAD;
                    continue;
                }

                // Проверка на выздоровление
                if (grid[y][x].infection_timer > strain.recovery_time) {
                    grid[y][x].state = HEALTHY;
                    grid[y][x].infection_timer = 0;
                    continue;
                }

                // Заражаем соседей (но не применяем сразу)
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue; // Пропускаем себя

                        int nx = x + dx;
                        int ny = y + dy;

                        // Проверяем границы и здоровье клетки
                        if (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT &&
                            grid[ny][nx].state == HEALTHY) {
                            
                            // Заражаем с вероятностью infection_rate
                            if ((rand() / (float)RAND_MAX) < strain.infection_rate) {
                                grid[ny][nx].state = INFECTED;
                                grid[ny][nx].strain_id = cell.strain_id;
                                grid[ny][nx].infection_timer = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    // Проверка на мутации (оставляем как было)
    int mutated_this_tick = 0;
    for (int i = 0; i < strain_count; i++) {
        if (!strains[i].has_mutated && strains[i].infected_count >= 500) {
            mutate_from_strain(i);
            strains[i].has_mutated = 1;
            mutated_this_tick = 1;
            break;
        }
    }

    // Глобальная мутация (если нужно)
    int infected = 0;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x].state == INFECTED) infected++;
        }
    }

    if (!mutated_this_tick && mutation_cooldown == 0 && infected >= 500 && strain_count < MAX_STRAINS) {
        mutate();
    }
}

void render_text(const char* text, int x, int y, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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

void render_grid(SDL_Renderer* renderer, TTF_Font* font) {
    if (mutation_fx_counter > 0) {
        mutation_fx_counter--;
        SDL_SetRenderDrawColor(renderer, 255, 255, 50, 255);
        SDL_RenderClear(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell cell = grid[y][x];
            if (cell.state == HEALTHY) {
                SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            } else if (cell.state == INFECTED) {
                Color c = strains[cell.strain_id].color;
                SDL_SetRenderDrawColor(renderer, (int)c.r, (int)c.g, (int)c.b, 255);
            } else if (cell.state == DEAD) {
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            }
            SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_Color white = {255, 255, 255, 255};
    char info[256];
    snprintf(info, sizeof(info), "Strains: %d", strain_count);
    render_text(info, 10, 10, white, renderer, font);

    int line = 30;
    for (int i = strain_count - 1; i >= 0 && i >= strain_count - 3; i--) {
        VirusStrain* s = &strains[i];
        snprintf(info, sizeof(info), "[%s] INF: %.2f | DEATH: %.2f | REC: %d",
                 s->name, s->infection_rate, s->death_rate, s->recovery_time);
        render_text(info, 10, line, white, renderer, font);
        line += 20;
    }

    SDL_RenderPresent(renderer);
}

void render_stats_window(SDL_Renderer* renderer, TTF_Font* font) {
    int healthy, infected, dead;
    count_stats(&healthy, &infected, &dead);

    int total = healthy + infected + dead;
    if (total == 0) return;

    float healthy_percent = healthy / (float)total;
    float infected_percent = infected / (float)total;
    float dead_percent = dead / (float)total;

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    int bar_width = 250;
    int start_y = 50;

    SDL_Rect bar = {25, start_y, bar_width * healthy_percent, 20};
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); SDL_RenderFillRect(renderer, &bar);

    bar.y += 30; bar.w = bar_width * infected_percent;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); SDL_RenderFillRect(renderer, &bar);

    bar.y += 30; bar.w = bar_width * dead_percent;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); SDL_RenderFillRect(renderer, &bar);

    char text[128];
    SDL_Color white = {255, 255, 255, 255};

    snprintf(text, sizeof(text), "Healthy: %d (%.1f%%)", healthy, healthy_percent * 100);
    render_text(text, 25, 20, white, renderer, font);
    snprintf(text, sizeof(text), "Infected: %d (%.1f%%)", infected, infected_percent * 100);
    render_text(text, 25, 50 + 60, white, renderer, font);
    snprintf(text, sizeof(text), "Dead: %d (%.1f%%)", dead, dead_percent * 100);
    render_text(text, 25, 80 + 60, white, renderer, font);

    SDL_RenderPresent(renderer);
}

void mutate_from_strain(int parent_id) {
    if (strain_count >= MAX_STRAINS) return;

    mutation_cooldown = 100;

    VirusStrain* prev = &strains[parent_id];
    VirusStrain* new_strain = &strains[strain_count++];

    snprintf(new_strain->name, sizeof(new_strain->name), "VRS-%d", strain_count - 1);
    new_strain->infection_rate = fminf(fmaxf(prev->infection_rate + ((rand() % 200 - 100) / 1000.0f), 0.01f), 0.9f);
    new_strain->death_rate = fminf(fmaxf(prev->death_rate + ((rand() % 100 - 50) / 1000.0f), 0.01f), 0.5f);
    new_strain->recovery_time = prev->recovery_time + (rand() % 41 - 20);
    if (new_strain->recovery_time < 100) new_strain->recovery_time = 100;
    new_strain->color = get_distinct_color(strain_count - 1);

    new_strain->has_mutated = 0;
    new_strain->infected_count = 0;

    mutation_fx_counter = 15;
    printf("[MUTATION] %s → %s | INF: %.2f | DEATH: %.2f | REC: %d\n",
           prev->name, new_strain->name, new_strain->infection_rate,
           new_strain->death_rate, new_strain->recovery_time);

    // Засеваем одну случайную клетку новым штаммом
    int x = rand() % GRID_WIDTH;
    int y = rand() % GRID_HEIGHT;
    grid[y][x].state = INFECTED;
    grid[y][x].strain_id = strain_count - 1;
    grid[y][x].infection_timer = 0;
}