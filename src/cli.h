#ifndef CLI_INCLUDED_
#define CLI_INCLUDED_

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
	bool print_info;
	unsigned long save_interval;
	unsigned long stopping_point;
	bool is_finite;
} options;

void parse_options(int argc, char *argv[]);

#endif /* CLI_INCLUDED_ */
