#include "world.h"
#include "graphics.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void next_frame(world_t *world)
{
	world_step(world);
	for (size_t x = 0; x < world_width(world); ++x) {
		for (size_t y = 0; y < world_height(world); ++y) {
			draw_cell(world_get(world, x, y), x, y);
		}
	}
	display_frame();
}

int main(int argc, char *argv[])
{
	jmp_buf jb;
	file_error_t err = FE_SUCCESS;
	world_t world;
	FILE *to;
	if (argc < 2) {
		fprintf(stderr, "Usage: piks <infile> <outfile>\n");
		return 1;
	}
	if (strcmp(argv[1], "-") != 0) {
		FILE *from = fopen(argv[1], "r");
		if ((err = setjmp(jb))) {
			fprintf(stderr, "Error when reading: ");
			file_error_print(err);
			fprintf(stderr, "\n");
			return err;
		}
		world_read(&world, from, jb);
		fclose(from);
	} else {
		world_init(&world, 100, 30, 300, time(NULL));
		world_populate(&world, -1);
	}
	to = fopen(argv[2], "w");
	begin_graphics();
	while (!sim_stopped()) {
		next_frame(&world);
		usleep(150000);
	}
	if ((err = setjmp(jb))) {
		end_graphics();
		fprintf(stderr, "Error when writing: ");
		file_error_print(err);
		fprintf(stderr, "\n");
	} else {
		world_write(&world, to, jb);
	}
	end_graphics();
	world_destroy(&world);
	fclose(to);
	return err;
}
