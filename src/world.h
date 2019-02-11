#ifndef WORLD_INCLUDED_
#define WORLD_INCLUDED_

#include "random.h"
#include "genome.h"
#include "animal.h"
#include "file.h"

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

#define world_width(world) ((world)->width)
#define world_height(world) ((world)->height)

void world_step(world_t *world);

void world_read(world_t *world, FILE *from, jmp_buf jb);

void world_write(const world_t *world, FILE *to, jmp_buf jb);

#endif /* WORLD_INCLUDED_ */
