#include "world.h"
#include "graphics.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
	world_t world;
	world_init(&world, 100, 50, time(NULL));
	world_populate(&world, 500);
	begin_graphics();
	for (size_t x = 0; x < world.width; ++x) {
		for (size_t y = 0; y < world.height; ++y) {
			draw_cell(world_get(&world, x, y), x, y);
		}
	}
	display_frame();
	getchar();
	end_graphics();
}
