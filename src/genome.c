#include "genome.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void genome_pool_init(genome_pool_t *pool, size_t count)
{
	if (count == 0) count = 1;
	pool->size = count;
	pool->slots = malloc(count * sizeof(*pool->slots));
	pool->freed = pool->slots;
	size_t i;
	for (i = 0; i < count - 1; ++i) {
		pool->slots[i].link.next_free = pool->slots + i + 1;
	}
	pool->slots[i].link.next_free = NULL;
}

void genome_pool_destroy(genome_pool_t *pool)
{
	free(pool->slots);
}

size_t genome_pool_get_count(const genome_pool_t *pool)
{
	return pool->size;
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
	if (!pool->freed) return NULL;
	gnm = pool->freed;
	pool->freed = gnm->link.next_free;
	gnm->link.pool = pool;
	gnm->refcnt = 0;
	return gnm;
}

genome_t *genome_random(genome_pool_t *pool, rand_t *seed)
{
	genome_t *gnm = genome_alloc(pool);
	if (!gnm) {
		return &pool->slots[next_random(seed) % pool->size];
	}
	for (size_t i = 0; i < 64; ++i) {
		gnm->actions[i] = *seed;
		next_random(seed);
	}
	return gnm;
}

genome_t *genome_mutant(genome_t *gnm, rand_t *seed)
{
	genome_pool_t *pool = gnm->link.pool;
	genome_t *mut = genome_alloc(pool);
	if (!mut) return gnm;
	memcpy(mut->actions, gnm->actions, 64);
	for (size_t i = 0; i < MUTATION_COUNT; ++i) {
		mut->actions[(i + *seed) % 64] ^= 1 << (*seed % 8);
		next_random(seed);
	}
	return mut;
}

void genome_inc(genome_t *gnm)
{
	++gnm->refcnt;
}

void genome_dec(genome_t *gnm)
{
	--gnm->refcnt;
	if (gnm->refcnt <= 0) {
		genome_pool_t *pool = gnm->link.pool;
		gnm->link.next_free = pool->freed;
		pool->freed = gnm;
	}
}

