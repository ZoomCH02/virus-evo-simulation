#ifndef VIRUS_SIM_H
#define VIRUS_SIM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>

// === Общие константы ===

// Размеры сетки (ширина x высота)
#define GRID_WIDTH 150
#define GRID_HEIGHT 100

// Размер одной клетки в пикселях
#define CELL_SIZE 8

// Размер окна, рассчитывается на основе сетки и размера клетки
#define WINDOW_WIDTH (GRID_WIDTH * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * CELL_SIZE)

// Максимальное количество различных штаммов вируса
#define MAX_STRAINS 10

// Количество записей мутаций, которые сохраняются в истории
#define MAX_MUTATION_HISTORY 10

// === Перечисления и структуры ===

// Состояние клетки
typedef enum {
    HEALTHY,    // Здорова
    INFECTED,   // Инфицирована
    DEAD        // Мертва
} CellState;

// RGB-цвет, используется для визуализации штаммов
typedef struct {
    float r, g, b;  // Значения цвета от 0.0 до 1.0
} Color;

// Структура описания вирусного штамма
typedef struct {
    char name[64];             // Название штамма
    float infection_rate;      // Вероятность заражения (0.0 - 1.0)
    float death_rate;          // Вероятность смерти инфицированного (0.0 - 1.0)
    float revival_rate;        // Вероятность "воскрешения" мёртвых клеток
    int recovery_time;         // Время восстановления после заражения
    Color color;               // Цвет для визуализации этого штамма
    int has_mutated;           // Флаг: мутировал ли штамм
    int infected_count;        // Кол-во текущих заражённых этим штаммом
} VirusStrain;

// Структура клетки в сетке
typedef struct {
    CellState state;     // Текущее состояние клетки
    int infection_timer; // Счётчик времени до восстановления/смерти
    int strain_id;       // ID штамма, если инфицирована
} Cell;

// Запись мутации, используется для отображения и истории
typedef struct {
    char message[256];   // Сообщение о мутации
    int strain_id;       // ID штамма, к которому относится мутация
} MutationRecord;

// === Глобальные переменные (декларации) ===

// Мьютексы для защиты данных при многопоточности
extern pthread_mutex_t grid_mutex;      // Мьютекс для доступа к сетке
extern pthread_mutex_t strains_mutex;   // Мьютекс для доступа к штаммам

// Управляющие флаги
extern int simulation_running;      // Флаг: идёт ли симуляция
extern int rendering_complete;      // Флаг: завершена ли отрисовка текущего кадра
extern int mutation_fx_counter;     // Счётчик эффектов мутаций
extern int mutation_cooldown;       // Задержка между мутациями

// Массив штаммов вирусов
extern VirusStrain strains[MAX_STRAINS];
extern int strain_count; // Текущее количество штаммов

// Основная сетка клеток (мир симуляции)
extern Cell grid[GRID_HEIGHT][GRID_WIDTH];

// История мутаций (для отображения)
extern MutationRecord mutation_history[MAX_MUTATION_HISTORY];
extern int mutation_history_count; // Количество записей в истории

#endif
