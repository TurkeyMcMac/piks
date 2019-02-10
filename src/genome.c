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

size_t genome_get_id(const genome_t *gnm)
{
	return (size_t)(gnm - gnm->link.pool->slots);
}

static bool genome_freed(const genome_pool_t *pool, size_t id)
{
	return pool->slots[id].link.pool != pool;
}

genome_t *genome_pool_get(genome_pool_t *pool, size_t id)
{
	if (id >= pool->size || genome_freed(pool, id)) return NULL;
	return &pool->slots[id];
}

static void read_genome(genome_pool_t *pool, FILE *from, jmp_buf jb)
{
	genome_t *gnm;
	uint32_t id = read_32(from, jb);
	if (id >= pool->size) longjmp(jb, FE_INVALID_GENOME_ID);
	gnm = &pool->slots[id];
	gnm->link.pool = pool;
	gnm->refcnt = 0;
	if (fread(gnm->actions, 1, 64, from) != 64)
		longjmp(jb, feof(from) ? FE_UNEXPECTED_EOF : FE_SYSTEM);
}

static void write_genome(const genome_pool_t *pool, size_t id, FILE *to,
	jmp_buf jb)
{
	write_32(id, to, jb);
	if (fwrite(pool->slots[id].actions, 1, 64, to) != 64)
		longjmp(jb, FE_SYSTEM);
}

void genome_pool_read(genome_pool_t *pool, FILE *from, jmp_buf jb)
{
	file_error_t err;
	jmp_buf local;
	size_t count = read_32(from, jb);
	pool->size = read_32(from, jb);
	pool->slots = calloc(pool->size, sizeof(*pool->slots));
	if ((err = setjmp(local))) {
		free(pool->slots);
		longjmp(jb, err);
	}
	while (count--) {
		read_genome(pool, from, local);
	}
	size_t i = 0;
	while (!genome_freed(pool, i)) {
		++i;
		if (i >= pool->size) return;
	}
	pool->freed = &pool->slots[i];
	while (++i < pool->size) {
		if (genome_freed(pool, i)) {
			pool->slots[i].link.next_free = pool->freed;
			pool->freed = &pool->slots[i];
		}
	}
}

void genome_pool_write(const genome_pool_t *pool, FILE *to, jmp_buf jb)
{
	size_t living = 0;
	for (size_t i = 0; i < pool->size; ++i) {
		if (!genome_freed(pool, i)) ++living;
	}
	write_32(living, to, jb);
	write_32(pool->size, to, jb);
	for (size_t i = 0; i < pool->size; ++i) {
		if (!genome_freed(pool, i)) {
			write_genome(pool, i, to, jb);
		}
	}
}
