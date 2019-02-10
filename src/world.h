#ifndef WORLD_INCLUDED_
#define WORLD_INCLUDED_

#include "random.h"
#include "genome.h"
#include "animal.h"

#define MUTATION_CHANCE 3 /* 3% */

typedef struct {
	genome_pool_t genomes;
	size_t width, height;
	rand_t rand;
	animal_t *cells;
} world_t;

void world_init(world_t *world,
	size_t width,
	size_t height,
	size_t genomes,
	rand_t seed);

void world_destroy(world_t *world);

void world_populate(world_t *world, size_t pop);

animal_t *world_get(world_t *world, size_t x, size_t y);

static size_t world_width(const world_t *world)
{ return world->width; }

static size_t world_height(const world_t *world)
{ return world->height; }

void world_step(world_t *world);

#endif /* WORLD_INCLUDED_ */
