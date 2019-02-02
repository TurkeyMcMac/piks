#include "genome.h"
#include <stdint.h>
#include <stdlib.h>

void genome_pool_init(genome_pool_t *pool)
{
	pool->alive = NULL;
	pool->dead = NULL;
}

static void free_genomes(genome_t *list)
{
	while (list) {
		genome_t *next = list->next;
		free(list);
		list = next;
	}
}

void genome_pool_destroy(genome_pool_t *pool)
{
	free_genomes(pool->alive);
	free_genomes(pool->dead);
}

action_t genome_get(const genome_t *gnm, input_t input)
{
	uint8_t byte, quart;
	byte = input >> 2;
	quart = input % 4;
	return (gnm->actions[byte] >> (quart * 2)) % 4;
}

genome_t *genome_alloc(genome_pool_t *pool)
{
	genome_t *gnm;
	if (pool->dead) {
		gnm = pool->dead;
		pool->dead = gnm->next;
	} else {
		gnm = malloc(sizeof(*gnm));
	}
	gnm->next = pool->alive;
	gnm->last = &pool->alive;
	pool->alive = gnm;
	if (gnm->next) {
		gnm->next->last = &gnm->next;
	}
	gnm->refcnt = 0;
	return gnm;
}

genome_t *genome_random(genome_pool_t *pool, rand_t *seed)
{
	genome_t *gnm = genome_alloc(pool);
	gnm->color.red = *seed;
	next_random(seed);
	gnm->color.green = *seed;
	next_random(seed);
	gnm->color.blue = *seed;
	next_random(seed);
	for (size_t i = 0; i < 64; ++i) {
		gnm->actions[i] = *seed;
		next_random(seed);
	}
	return gnm;
}

genome_t *genome_clone(genome_t *gnm)
{
	genome_t *clone = genome_alloc(gnm->pool);
	clone->color = gnm->color;
	memcpy(clone->actions, gnm->actions, 64);
	return clone;
}

void genome_inc(genome_t *gnm)
{
	++gnm->refcnt;
}

void genome_dec(genome_t *gnm)
{
	--gnm->refcnt;
	if (gnm->refcnt <= 0) {
		*gnm->last = gnm->next;
		if (gnm->next) {
			gnm->next->last = gnm->last;
		}
		gnm->next = gnm->pool->dead;
		gnm->pool->dead = gnm;
	}
}

void genome_mutate(genome_t *gnm, rand_t *seed)
{
	/* Mutate color */
	switch (*seed % 3) {
	case 0:
		gnm->color.red = *seed;
		break;
	case 1:
		gnm->color.green = *seed;
		break;
	case 2:
		gnm->color.blue = *seed;
		break;
	}
	/* Mutate actions */
	for (size_t i = 0; i < MUTATION_COUNT; ++i) {
		gnm->actions[(i + *seed) % 64] ^= 1 << (*seed % 8);
		next_random(seed);
	}
}
