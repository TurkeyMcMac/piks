#include "graphics.h"
#include <curses.h>

#define NULL_PAIR 1
#define ANIMAL_PAIR 2

void begin_graphics(void)
{
	initscr();
	start_color();
	init_pair(NULL_PAIR, COLOR_BLACK, COLOR_BLUE);
	init_pair(ANIMAL_PAIR, COLOR_RED, COLOR_WHITE);
}

void end_graphics(void)
{
	endwin();
}

static void draw_null(size_t x, size_t y)
{
	if (has_colors()) {
		attron(COLOR_PAIR(NULL_PAIR));
		mvaddch(y, x, '#');
		attroff(COLOR_PAIR(NULL_PAIR));
	}
}

static void draw_animal(size_t x, size_t y, direction_t dir)
{
	char icon = ' ';
	switch (dir) {
	case DIRECTION_RIGHT:
		icon = '>';
		break;
	case DIRECTION_UP:
		icon = '^';
		break;
	case DIRECTION_LEFT:
		icon = '<';
		break;
	case DIRECTION_DOWN:
		icon = 'v';
		break;
	}
	attron(COLOR_PAIR(ANIMAL_PAIR));
	mvaddch(y, x, icon | A_BOLD);
	attroff(COLOR_PAIR(ANIMAL_PAIR));
}

void draw_cell(const animal_t *cell, size_t x, size_t y)
{
	if (animal_is_null(cell)) {
		draw_null(x, y);
	} else {
		draw_animal(x, y, animal_get_direction(cell));
	}
}

void display_frame(void)
{
	refresh();
}
