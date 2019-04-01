#include "random.h"
#include <stddef.h>

extern struct options {
	const char *input, *output;
	size_t width, height;
	size_t population;
	rand_t seed;
} options;

void parse_options(int argc, char *argv[]);
