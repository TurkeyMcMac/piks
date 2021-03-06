#include "world.h"
#include "ui.h"
#include "file.h"
#include "cli.h"
#include "ticker.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int try_read(char *progname, world_t *world, FILE *from)
{
	jmp_buf jb;
	int err;
	if ((err = setjmp(jb))) {
		fprintf(stderr, "%s: Error when reading: ", progname);
		file_error_print(err);
		fprintf(stderr, "\n");
		return err;
	} else {
		world_read(world, from, jb);
		return 0;
	}
}

static int try_write(char *progname, world_t *world)
{
	jmp_buf jb;
	int err;
	FILE *to = fopen(options.output, "w");
	if (!to) {
		fprintf(stderr, "%s: Could not write file '%s': %s\n",
			progname, options.output, strerror(errno));
		return FE_SYSTEM;
	}
	if ((err = setjmp(jb)) == 0) {
		world_write(world, to, jb);
	} else {
		int errnum = errno;
		fprintf(stderr, "%s: Error when writing: ", progname);
		errno = errnum;
		file_error_print(err);
		fprintf(stderr, "\n");
	}
	fclose(to);
	return err;
}

static void print_genome(genome_t *gnm)
{
	printf("%5zu  %10ld  ",
		genome_get_id(gnm), genome_population(gnm));
	uint8_t *seq = genome_bytes(gnm);
	for (size_t i = 0; i < genome_length(gnm); ++i) {
		printf("%02X", seq[i]);
	}
	putchar('\n');
}

int main(int argc, char *argv[])
{
	unsigned long tick = 0;
	world_t world;
	parse_options(argc, argv);
	if (options.input) {
		FILE *from = fopen(options.input, "r");
		if (!from) {
			fprintf(stderr, "%s: Could not read file '%s': %s\n",
				argv[0], options.input, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (try_read(argv[0], &world, from)) exit(EXIT_FAILURE);
		fclose(from);
	} else {
		world_init(&world, options.width, options.height,
			options.population, options.seed);
		world_populate(&world, options.population);
	}
	if (options.print_info) {
		printf(	"Format-version: %lu\n"
			"Width: %zu\n"
			"Height: %zu\n"
			"Population: %zu\n"
			"Random-state: %"PRIu32"\n"
			"Species: %zu\n"
			"   ID  Population  Sequence\n"
			,
			(unsigned long)FILE_FORMAT_VERSION,
			world_width(&world),
			world_height(&world),
			world_population(&world),
			world_random_state(&world),
			world_count_species(&world));
		for (genome_t *gnm = world_first_alive(&world);
		     gnm;
		     gnm = genome_next_alive(gnm)) {
			print_genome(gnm);
		}
		world_destroy(&world);
		exit(0);
	}
	begin_ui(options.do_ui);
	atexit(end_ui);
	ticker_t ticker;
	ticker_init(&ticker, options.frame_time);
	while (!sim_stopped()
	    && !(options.is_finite && tick >= options.stopping_point)) {
		++tick;
		if (options.save_interval && tick % options.save_interval == 0)
		{
			if (try_write(argv[0], &world)) exit(EXIT_FAILURE);
		}
		world_step(&world);
		if (options.do_ui) {
			for (size_t x = 0; x < world_width(&world); ++x) {
				for (size_t y = 0;
				     y < world_height(&world);
				     ++y) {
					draw_cell(
						world_get(&world, x, y), x, y);
				}
			}
			display_frame();
			ticker_step(&ticker);
		}
	}
	if (try_write(argv[0], &world)) exit(EXIT_FAILURE);
	world_destroy(&world);
	exit(0);
}
