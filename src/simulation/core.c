#include "core.h"
#include "grid.h"
#include "mutation.h"
#include <unistd.h>

// Инициализация мьютексов и флагов
pthread_mutex_t grid_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t strains_mutex = PTHREAD_MUTEX_INITIALIZER;
int simulation_running = 1;
int rendering_complete = 1;
int mutation_fx_counter = 0;
int mutation_cooldown = 0;


void init_simulation() {
    init_strain();
    init_grid();
    
    pthread_t sim_thread;
    pthread_create(&sim_thread, NULL, simulation_thread, NULL);
}

void reset_simulation() {
    pthread_mutex_lock(&grid_mutex);
    pthread_mutex_lock(&strains_mutex);
    init_strain();
    init_grid();
    pthread_mutex_unlock(&strains_mutex);
    pthread_mutex_unlock(&grid_mutex);
}

void cleanup_simulation() {
    simulation_running = 0;
    pthread_mutex_destroy(&grid_mutex);
    pthread_mutex_destroy(&strains_mutex);
}

void* simulation_thread(void* arg) {
    (void)arg;
    while (simulation_running) {
        while (!rendering_complete && simulation_running) usleep(1000);
        if (!simulation_running) break;
        
        pthread_mutex_lock(&grid_mutex);
        pthread_mutex_lock(&strains_mutex);
        update_grid();
        pthread_mutex_unlock(&strains_mutex);
        pthread_mutex_unlock(&grid_mutex);
        
        rendering_complete = 0;
        usleep(10000);
    }
    return NULL;
}

void update_grid() {
    // Сначала обновляем все состояния в ОДИН проход
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell cell = grid[y][x];

            if (cell.state == HEALTHY) {
                // Здоровые клетки могут оживлять мертвых соседей
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;

                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT &&
                            grid[ny][nx].state == DEAD) {
                            
                            if ((rand() / (float)RAND_MAX) < strains[cell.strain_id].revival_rate) {
                                grid[ny][nx].state = HEALTHY;
                                grid[ny][nx].infection_timer = 0;
                            }
                        }
                    }
                }
            }

            else if (cell.state == INFECTED) {
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

    // Проверка на мутации
    for (int i = 0; i < strain_count; i++) {
        if (!strains[i].has_mutated && strains[i].infected_count >= 500) {
            mutate_from_strain(i);
            strains[i].has_mutated = 1;
            break;
        }
    }
}
