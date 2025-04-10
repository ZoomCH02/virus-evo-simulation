#ifndef CORE_H
#define CORE_H

#include "../../include/virus_sim.h"

void init_simulation();
void reset_simulation();
void cleanup_simulation();
void* simulation_thread(void* arg);
void update_grid();

#endif