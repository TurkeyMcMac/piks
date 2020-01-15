#ifndef GRAPHICS_INCLUDED_
#define GRAPHICS_INCLUDED_

#include "animal.h"
#include <stdbool.h>
#include <stddef.h>

void begin_ui(bool do_graphics);

void end_ui(void);

bool sim_stopped(void);

void draw_cell(const animal_t *cell, size_t x, size_t y);

void display_frame(void);

#endif /* GRAPHICS_INCLUDED_ */
