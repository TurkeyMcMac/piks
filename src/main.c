#include "world.h"
#include "graphics.h"
#include "file.h"
#include "cli.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int try_read(world_t *world, FILE *from)
{
	jmp_buf jb;
	int err;
	if ((err = setjmp(jb))) {
		fprintf(stderr, "Error when reading: ");
		file_error_print(err);
		fprintf(stderr, "\n");
		return err;
	} else {
		world_read(world, from, jb);
		return 0;
	}
}

static int try_write(world_t *world, FILE *to)
{
	jmp_buf jb;
	int err;
	if ((err = setjmp(jb))) {
		fprintf(stderr, "Error when writing: ");
		file_error_print(err);
		fprintf(stderr, "\n");
		return err;
	} else {
		world_write(world, to, jb);
		return 0;
	}
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
		if (try_read(&world, from)) exit(EXIT_FAILURE);
		fclose(from);
	} else {
		world_init(&world, options.width, options.height,
			options.population, options.seed);
		world_populate(&world, options.population);
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
			try_write(&world, to);
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
	try_write(&world, to);
	end_graphics();
	world_destroy(&world);
	fclose(to);
	exit(0);
}
