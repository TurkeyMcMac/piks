#include "world.h"
#include "graphics.h"
#include "file.h"
#include "cli.h"
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

static int try_write(char *progname, world_t *world, FILE *to)
{
	jmp_buf jb;
	int err;
	if ((err = setjmp(jb))) {
		fprintf(stderr, "%s: Error when writing: ", progname);
		file_error_print(err);
		fprintf(stderr, "\n");
		return err;
	} else {
		world_write(world, to, jb);
		return 0;
	}
}

static void print_genome(genome_t *gnm)
{
	printf("%5ld  %10zu  ",
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
	FILE *to;
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
	to = fopen(options.output, "w");
	if (!to) {
		fprintf(stderr, "%s: Could not write file '%s': %s\n",
			argv[0], options.output, strerror(errno));
		exit(EXIT_FAILURE);
	}
	begin_graphics();
	while (!sim_stopped()) {
		++tick;
		if (options.save_interval && tick % options.save_interval == 0)
		{
			try_write(argv[0], &world, to);
		}
		world_step(&world);
		if (options.do_graphics) {
			for (size_t x = 0; x < world_width(&world); ++x) {
				for (size_t y = 0;
				     y < world_height(&world);
				     ++y) {
					draw_cell(
						world_get(&world, x, y), x, y);
				}
			}
			display_frame();
			usleep(options.frame_time);
		}
	}
	try_write(argv[0], &world, to);
	end_graphics();
	world_destroy(&world);
	fclose(to);
	exit(0);
}
