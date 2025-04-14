#ifndef MUTATION_H
#define MUTATION_H

#include "../../include/virus_sim.h"

void init_strain();
void mutate_from_strain(int parent_id);
Color get_distinct_color(int index);

#endif