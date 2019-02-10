#ifndef GENOME_INCLUDED_
#define GENOME_INCLUDED_

#include "action.h"
#include "random.h"
#include "file.h"
#include <stddef.h>
#include <stdint.h>

#define MUTATION_COUNT 4
#define MIN_POOL_SIZE 10

struct genome_pool;

typedef struct genome {
	union {
		struct genome_pool *pool;
		struct genome *next_free;
	} link;
	long refcnt;
	uint8_t actions[64];
} genome_t;

typedef struct genome_pool {
	genome_t *slots, *freed;
	size_t size;
} genome_pool_t;

void genome_pool_init(genome_pool_t *pool, size_t count);

void genome_pool_destroy(genome_pool_t *pool);

size_t genome_pool_get_count(const genome_pool_t *pool);

genome_t *genome_alloc(genome_pool_t *pool);

genome_t *genome_random(genome_pool_t *pool, rand_t *seed);

genome_t *genome_mutant(genome_t *gnm, rand_t *seed);

void genome_inc(genome_t *gnm);

void genome_dec(genome_t *gnm);

action_t genome_get(const genome_t *gnm, input_t input);

size_t genome_get_id(const genome_t *gnm);

genome_t *genome_pool_get(genome_pool_t *pool, size_t id);

void genome_pool_read(genome_pool_t *pool, FILE *from, jmp_buf jb);

void genome_pool_write(const genome_pool_t *pool, FILE *to, jmp_buf jb);

#endif /* GENOME_INCLUDED_ */
