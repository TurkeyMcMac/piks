#include "random.h"
#include <stddef.h>
#include <stdbool.h>

extern struct options {
	const char *input, *output;
	size_t width, height;
	size_t population;
	rand_t seed;
	long frame_time;
	bool do_graphics;
} options;

void parse_options(int argc, char *argv[]);
