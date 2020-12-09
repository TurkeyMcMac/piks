#include "ui.h"
#include <curses.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

static bool use_graphics;

static volatile sig_atomic_t stop_signalled = false;
static void stop_signal_handler(int s)
{
	(void)s;
	stop_signalled = true;
}

void begin_ui(bool do_graphics)
{
	use_graphics = do_graphics;
	if (use_graphics) {
		initscr();
		nodelay(stdscr, true);
	} else {
		fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	}
	struct sigaction sa = { .sa_handler = stop_signal_handler };
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

void end_ui(void)
{
	if (use_graphics) endwin();
}

bool sim_stopped(void)
{
	if (stop_signalled) {
		return true;
	} else if (use_graphics) {
		return getch() != ERR;
	} else {
		char buf[1];
		return read(STDIN_FILENO, buf, 1) == 1;
	}
}

static void draw_null(size_t x, size_t y)
{
	mvaddch(y, x, '`');
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
	mvaddch(y, x, A_REVERSE | A_BOLD | icon);
}

void draw_cell(const animal_t *cell, size_t x, size_t y)
{
	if (use_graphics) {
		if (animal_is_null(cell)) {
			draw_null(x, y);
		} else {
			draw_animal(x, y, animal_get_direction(cell));
		}
	}
}

void display_frame(void)
{
	if (use_graphics) refresh();
}
