#ifndef GRAPHICS_INCLUDED_
#define GRAPHICS_INCLUDED_

#include "animal.h"
#include <stddef.h>

void begin_graphics(void);

void end_graphics(void);

void draw_cell(const animal_t *cell, size_t x, size_t y);

void display_frame(void);

#endif /* GRAPHICS_INCLUDED_ */
