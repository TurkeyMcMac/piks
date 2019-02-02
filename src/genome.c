#include "genome.h"
#include "random.h"
#include <stdint.h>
#include <stdlib.h>

void genome_pool_init(genome_pool_t *pool, size_t size)
{
	if (size < MIN_POOL_SIZE) {
		pool->size = MIN_POOL_SIZE;
	} else {
		pool->size = size;
	}
	pool->next = 0;
	pool->slots = malloc(pool->size * sizeof(*pool->slots));
}

void genome_pool_destroy(genome_pool_t *pool)
{
	free(pool->slots);
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
	if (pool->freed) {
		gnm = pool->freed;
		pool->freed = pool->freed->manage.next_free;
	} else {
		if (pool->next >= pool->size) {
			pool->size = pool->size * 3 / 2;
			pool->slots = realloc(pool->slots,
				pool->size * sizeof(*pool->slots));
			if (!pool->slots) {
				/* TODO: throw an error. */
			}
		}
		gnm = &pool->slots[pool->next];
		++pool->next;
	}
	gnm->manage.pool = pool;
	gnm->refcnt = 1;
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
	genome_t *clone = genome_alloc(gnm->manage.pool);
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
	if (gnm->refcnt <= 0) {
		genome_pool_t *pool = gnm->manage.pool;
		gnm->manage.next_free = pool->next;
		pool->next = gnm;
	} else {
		--gnm->refcnt;
	}
}

void genome_mutate(genome_t *gnm, rand_t *seed)
{
	/* Mutate color */
	switch (seed % 3) {
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
		gnm->[(i + *seed) % 64] ^= 1 << (*seed % 8);
		next_random(seed);
	}
}
