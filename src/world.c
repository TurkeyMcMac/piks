#include "world.h"
#include <assert.h>
#include <stdlib.h>

void world_init(world_t *world, size_t width, size_t height, rand_t seed)
{
	genome_pool_init(&world->genomes);
	world->width = width;
	world->height = height;
	world->rand = seed;
	world->cells = calloc(width * height, sizeof(*world->cells));
}

void world_destroy(world_t *world)
{
	genome_pool_destroy(&world->genomes);
	free(world->cells);
}

void world_populate(world_t *world, size_t pop)
{
	scatter_t scat; 
	scatter_init(&scat, world->width * world->height, pop);
	while (scatter_has_next(&scat)) {
		size_t i = scatter_next(&scat, &world->rand);
		genome_t *gnm = genome_random(&world->genomes, &world->rand);
		direction_t dir = next_random(&world->rand) % DIRECTION_NUMBER;
		animal_init(&world->cells[i], gnm, dir);
	}
	size_t j = 0;
	for (size_t x = 0; x < world->width; ++x) {
		for (size_t y = 0; y < world->height; ++y) {
			if (!animal_is_null(world_get(world, x, y))) ++j;
		}
	}
	assert(j == pop);
}

animal_t *world_get(world_t *world, size_t x, size_t y)
{
	assert(x < world->width);
	assert(y < world->height);
	return &world->cells[y * world->width + x];
}

void world_step(world_t *world)
{
	/* TODO: implement this */
}
