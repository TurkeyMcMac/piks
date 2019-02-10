#include "world.h"
#include "graphics.h"
#include <stdio.h>
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
	world_t world;
	world_init(&world, 100, 30, 300, time(NULL));
	world_populate(&world, -1);
	world_step(&world);
	begin_graphics();
	while (1) {
		next_frame(&world);
		usleep(150000);
	}
	getchar();
	end_graphics();
}
