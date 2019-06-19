#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

struct options options;

static const char help[] =
"Usage: %s [options]\n"
"\n"
"Options:\n"
"  -W <width>       Set the world width.\n"
"  -H <height>      Set the world height.\n"
"  -i <file>        Set the input world file.\n"
"  -o <file>        Set the destination, where to write the world.\n"
"  -p <population>  Set the population/number of species in the world.\n"
"  -r <seed>        Set the random seed for the world.\n"
"  -f <FPS>         Set the frames per second. The default is 30.\n"
"  -G               Turn graphics on. This is the default.\n"
"  -g               Turn graphics off. -f has no effect in this case.\n"
"  -I               Print world info and exit. All this information must be \n"
"                   determinable by other options such as -i, -W, etc.\n"
"  -s <interval>    Save every <interval> ticks. 0 means no saving except at\n"
"                   the end. This is the default.\n"
"  -h               Print this help and exit.\n"
"  -v               Print version information and exit.\n"
"The options -W, -H, -p, and -r are only used if -i is not provided. With no\n"
"-i option, a new world is created rather than loading one from a file. If -o\n"
"is not provided but -i is, the file specified by -i will also be written as\n"
"the save location.\n"
;
static const char version[] = "%s version 0.3.3\n";

static unsigned long non_neg_arg(char *progname)
{
	char *num_end;
	long parsed;
	if (!optarg) {
		fprintf(stderr, "%s: Expected numeric argument\n", progname);
		exit(EXIT_FAILURE);
	}
	parsed = strtol(optarg, &num_end, 10);
	if (*num_end) {
		fprintf(stderr, "%s: Invalid numeric argument: %s\n",
			progname, optarg);
		exit(EXIT_FAILURE);
	}
	if (parsed < 0) {
		fprintf(stderr, "%s: Expected non-negative numeric argument\n",
			progname);
		exit(EXIT_FAILURE);
	}
	if (parsed > UINT32_MAX) {
		fprintf(stderr, "%s: Number must be less than %lu\n",
			progname, (unsigned long)UINT32_MAX);
		exit(EXIT_FAILURE);
	}
	return (unsigned long)parsed;
}

static void error_not_set(char *progname, const char *what, char option)
{
	fprintf(stderr, "%s: %s not set (use -%c <%s>)\n",
		progname, what, option, what);
}

#define FPS(fps) (1e6 / (fps))

void parse_options(int argc, char *argv[]) {
	bool failed = false;
	bool width_set = false, height_set = false;
	bool input_set = false, output_set = false;
	bool pop_set = false;
	bool seed_set = false;
	static const char opts[] =
		"W:" // width
		"H:" // height
		"i:" // input
		"o:" // output
		"p:" // population
		"r:" // seed
		"f:" // framerate
		"G"  // graphics on
		"g"  // graphics off
		"s:" // save interval
		"I"  // just print world info
		"h"  // help
		"v"  // version
	;
	char *progname = argv[0];
	int opt;
	if (argc <= 1) {
		fprintf(stderr, help, progname);
		exit(EXIT_FAILURE);
	}
	options.input = NULL;
	options.frame_time = FPS(30);
	options.do_graphics = true;
	options.print_info = false;
	options.save_interval = 0;
	while ((opt = getopt(argc, argv, opts)) != -1) {
		switch (opt) {
		case 'W':
			options.width = non_neg_arg(progname);
			width_set = true;
			break;
		case 'H':
			options.height = non_neg_arg(progname);
			height_set = true;
			break;
		case 'i':
			options.input = optarg;
			input_set = optarg != NULL;
			break;
		case 'o':
			options.output = optarg;
			output_set = optarg != NULL;
			break;
		case 'p':
			options.population = non_neg_arg(progname);
			pop_set = true;
			break;
		case 'r':
			options.seed = non_neg_arg(progname);
			seed_set = true;
			break;
		case 'f':
			options.frame_time = FPS(non_neg_arg(progname));
			break;
		case 'G':
			options.do_graphics = true;
			break;
		case 'g':
			options.do_graphics = false;
			break;
		case 's':
			options.save_interval = non_neg_arg(progname);
			break;
		case 'I':
			options.print_info = true;
			break;
		case 'h':
			printf(help, progname);
			exit(0);
		case 'v':
			printf(version, progname);
			exit(0);
		default:
			fprintf(stderr, help, progname);
			exit(EXIT_FAILURE);
		}
	}
	if (!input_set) {
		if (!width_set) {
			error_not_set(progname, "world width", 'W');
			failed = true;
		}
		if (!height_set) {
			error_not_set(progname, "world height", 'H');
			failed = true;
		}
		if (!pop_set) {
			error_not_set(progname, "population", 'p');
			failed = true;
		}
	}
	if (options.print_info) {
		if (failed) {
			error_not_set(progname, "input file", 'i');
			failed = true;
		}
	} else if (!output_set) {
		if (input_set) {
			options.output = options.input;
			output_set = true;
		} else {
			if (failed) error_not_set(progname, "input file", 'i');
			error_not_set(progname, "output file", 'o');
			failed = true;
		}
	}
	if (failed) exit(EXIT_FAILURE);
	if (!input_set && !seed_set) options.seed = time(NULL);
}
