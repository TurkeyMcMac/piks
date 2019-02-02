#ifndef GENOME_INCLUDED_
#define GENOME_INCLUDED_

#include "action.h"
#include "color.h"
#include <stddef.h>
#include <stdint.h>

#define MUTATION_COUNT 4
#define MIN_POOL_SIZE 10

typedef struct {
	long refcnt;
	union {
		genome_t *next_free;
		genome_pool_t *pool;
	} manage;
	color_t color;
	uint8_t actions[64];
} genome_t;

typedef struct {
	genome_t *slots;
	size_t next;
	size_t size;
} genome_pool_t;

void genome_pool_init(genome_pool_t *pool, size_t size);

void genome_pool_destroy(genome_pool_t *pool);

genome_t *genome_alloc(genome_pool_t *pool);

genome_t *genome_random(genome_pool_t *pool, rand_t *seed);

genome_t *genome_clone(genome_t *gnm);

void genome_mutate(genome_t *gnm, rand_t *seed);

void genome_inc(genome_t *gnm);

void genome_dec(genome_t *gnm);

action_t genome_get(const genome_t *gnm, input_t input);

#endif /* GENOME_INCLUDED_ */
