#include "mutation.h"
#include <stdio.h>
#include <math.h>

int mutation_history_count = 0;
int strain_count = 1;
MutationRecord mutation_history[MAX_MUTATION_HISTORY] = {0};
Cell grid[GRID_HEIGHT][GRID_WIDTH];
VirusStrain strains[MAX_STRAINS];

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
        {0.5f, 1.0f, 0.8f}, // Аквамариновый
        {0.9f, 0.2f, 0.4f}  // Малиновый
    };
    int size = sizeof(palette) / sizeof(palette[0]);
    return palette[index % size];
}


void init_strain() {
    strain_count = 1;
    mutation_history_count = 0;
    for (int i = 0; i < MAX_STRAINS; i++) {
        strains[i] = (VirusStrain){0};
    }
    strains[0] = (VirusStrain){
        .name = "VRS-0",
        .infection_rate = 0.05f,
        .death_rate = 0.005f,
        .revival_rate = 0.001f,
        .recovery_time = 200,
        .color = {1.0f, 0.0f, 0.0f},
        .has_mutated = 0,
        .infected_count = 0
    };
}

void mutate_from_strain(int parent_id) {
    if (strain_count >= MAX_STRAINS) return;

    mutation_cooldown = 100;

    VirusStrain* prev = &strains[parent_id];
    VirusStrain* new_strain = &strains[strain_count++];

    snprintf(new_strain->name, sizeof(new_strain->name), "VRS-%d", strain_count - 1);
    new_strain->infection_rate = fminf(fmaxf(prev->infection_rate + ((rand() % 200 - 100) / 1000.0f), 0.01f), 0.9f);
    new_strain->death_rate = fminf(fmaxf(prev->death_rate + ((rand() % 100 - 50) / 1000.0f), 0.01f), 0.5f);
    new_strain->revival_rate = fminf(fmaxf(prev->revival_rate + ((rand() % 50 - 25) / 1000.0f), 0.0f), 0.1f);
    new_strain->recovery_time = prev->recovery_time + (rand() % 41 - 20);
    if (new_strain->recovery_time < 100) new_strain->recovery_time = 100;
    new_strain->color = get_distinct_color(strain_count - 1);

    new_strain->has_mutated = 0;
    new_strain->infected_count = 0;

    mutation_fx_counter = 15;
    
    // Добавляем запись в историю мутаций
    if (mutation_history_count >= MAX_MUTATION_HISTORY) {
        // Сдвигаем историю, если достигли максимума
        for (int i = 0; i < MAX_MUTATION_HISTORY-1; i++) {
            mutation_history[i] = mutation_history[i+1];
        }
        mutation_history_count = MAX_MUTATION_HISTORY-1;
    }
    
    snprintf(mutation_history[mutation_history_count].message, sizeof(mutation_history[0].message), 
             "%s -> %s\nINF: %.2f -> %.2f\nDEATH: %.2f -> %.2f\nREC: %d -> %d",
             prev->name, new_strain->name, 
             prev->infection_rate, new_strain->infection_rate,
             prev->death_rate, new_strain->death_rate,
             prev->recovery_time, new_strain->recovery_time);
             
    mutation_history[mutation_history_count].strain_id = strain_count - 1;
    mutation_history_count++;

    // Засеваем одну случайную клетку новым штаммом
    int x = rand() % GRID_WIDTH;
    int y = rand() % GRID_HEIGHT;
    grid[y][x].state = INFECTED;
    grid[y][x].strain_id = strain_count - 1;
    grid[y][x].infection_timer = 0;
}
