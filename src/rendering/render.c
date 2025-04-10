#include "render.h"
#include "../simulation/grid.h"
#include <string.h>

void render_text(const char* text, int x, int y, SDL_Color color, 
                SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_simulation(SDL_Renderer* renderer, SDL_Renderer* stats_renderer, TTF_Font* font) {
    // Отрисовка основного окна
    if (mutation_fx_counter > 0) {
        mutation_fx_counter--;
        SDL_SetRenderDrawColor(renderer, 255, 255, 50, 255);
        SDL_RenderClear(renderer);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    pthread_mutex_lock(&grid_mutex);
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell cell = grid[y][x];
            if (cell.state == HEALTHY) {
                SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            } else if (cell.state == INFECTED) {
                pthread_mutex_lock(&strains_mutex);
                Color c = strains[cell.strain_id].color;
                pthread_mutex_unlock(&strains_mutex);
                SDL_SetRenderDrawColor(renderer, 
                                      (int)(c.r * 255), 
                                      (int)(c.g * 255), 
                                      (int)(c.b * 255), 
                                      255);
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
    pthread_mutex_lock(&strains_mutex);
    for (int i = strain_count - 1; i >= 0 && i >= strain_count - 3; i--) {
        VirusStrain* s = &strains[i]; // Объявляем переменную s здесь
        snprintf(info, sizeof(info), "[%s] INF: %.2f | DEATH: %.2f | REV: %.3f | REC: %d",
                 s->name, s->infection_rate, s->death_rate, s->revival_rate, s->recovery_time);
        render_text(info, 10, line, white, renderer, font);
        line += 20;
    }
    pthread_mutex_unlock(&strains_mutex);
    
    pthread_mutex_unlock(&grid_mutex);
    
    SDL_RenderPresent(renderer);
    rendering_complete = 1;


    // Отрисовка окна статистики
    render_stats_window(stats_renderer, font);
}

void render_stats_window(SDL_Renderer* renderer, TTF_Font* font) {
    int healthy, infected, dead;

    pthread_mutex_lock(&grid_mutex);
    count_stats(&healthy, &infected, &dead);
    pthread_mutex_unlock(&grid_mutex);

    count_stats(&healthy, &infected, &dead);

    int total = healthy + infected + dead;
    if (total == 0) return;

    float healthy_percent = healthy / (float)total;
    float infected_percent = infected / (float)total;
    float dead_percent = dead / (float)total;

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Основная статистика (верхняя часть окна)
    int bar_width = 250;
    SDL_Rect bar = {25, 20, bar_width * healthy_percent, 20};
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); SDL_RenderFillRect(renderer, &bar);

    bar.y = 50; bar.w = bar_width * infected_percent;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); SDL_RenderFillRect(renderer, &bar);

    bar.y = 80; bar.w = bar_width * dead_percent;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); SDL_RenderFillRect(renderer, &bar);

    char text[128];
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color strain_colors[MAX_STRAINS];

    // Заполняем цвета штаммов для отображения
    for (int i = 0; i < strain_count; i++) {
        strain_colors[i] = (SDL_Color){
            (Uint8)(strains[i].color.r * 255),
            (Uint8)(strains[i].color.g * 255),
            (Uint8)(strains[i].color.b * 255),
            255
        };
    }

    // Основная статистика текстом
    render_text("Population Status:", 25, 0, yellow, renderer, font);
    snprintf(text, sizeof(text), "Healthy: %d (%.1f%%)", healthy, healthy_percent * 100);
    render_text(text, 25, 20, white, renderer, font);
    snprintf(text, sizeof(text), "Infected: %d (%.1f%%)", infected, infected_percent * 100);
    render_text(text, 25, 50, white, renderer, font);
    snprintf(text, sizeof(text), "Dead: %d (%.1f%%)", dead, dead_percent * 100);
    render_text(text, 25, 80, white, renderer, font);

    // История мутаций (нижняя часть окна)
    int mutation_start_y = 110;
    render_text("Mutation History:", 25, mutation_start_y, yellow, renderer, font);
    
    int y_pos = mutation_start_y + 20;
    int max_mutations_to_show = 5; // Сколько последних мутаций показывать
    
    for (int i = mutation_history_count - 1; 
         i >= 0 && i >= mutation_history_count - max_mutations_to_show; 
         i--) {
        
        // Цвет штамма для иконки
        SDL_SetRenderDrawColor(renderer, 
                              strain_colors[mutation_history[i].strain_id].r,
                              strain_colors[mutation_history[i].strain_id].g,
                              strain_colors[mutation_history[i].strain_id].b,
                              255);
        SDL_Rect strain_icon = {25, y_pos + 5, 10, 10};
        SDL_RenderFillRect(renderer, &strain_icon);
        
        // Первая строка - номер мутации
        snprintf(text, sizeof(text), "Mutation #%d:", i+1);
        render_text(text, 40, y_pos, white, renderer, font);
        y_pos += 20;
        
        // Разбиваем сообщение на строки
        char* line = strtok(mutation_history[i].message, "\n");
        while (line != NULL && y_pos < 300) {
            render_text(line, 40, y_pos, white, renderer, font);
            y_pos += 20;
            line = strtok(NULL, "\n");
        }
        
        y_pos += 10; // Отступ между мутациями
    }

    SDL_RenderPresent(renderer);
}

