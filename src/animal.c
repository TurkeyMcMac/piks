#include "animal.h"

void animal_init(animal_t *an, genome_t *gnm, direction_t dir)
{
	genome_inc(gnm);
	an->genome = gnm;
	an->direction = dir;
	an->flags = 0;
}

void animal_clone(animal_t *src, animal_t *dst)
{
	*dst = *src;
	genome_inc(dst->genome);
}

void animal_null(animal_t *an)
{
	an->genome = NULL;
}

bool animal_is_null(const animal_t *an)
{
	return an->genome == NULL;
}

void animal_mutant(animal_t *src, animal_t *dst, rand_t *seed)
{
	genome_t *mut = genome_mutant(src->genome, seed);
	animal_init(dst, mut, src->direction);
}

action_t animal_get_action(const animal_t *an, input_t input)
{
	return genome_get(an->genome, input);
}

void animal_turn(animal_t *an, int quarts)
{
	an->direction += quarts;
	an->direction &= 0x3;
}

direction_t animal_get_direction(const animal_t *an)
{
	return an->direction;
}

uint16_t *animal_flags(animal_t *an)
{
	return &an->flags;
}

void animal_die(animal_t *an)
{
	genome_dec(an->genome);
	animal_null(an);
}

void animal_read(animal_t *an, genome_pool_t *pool, FILE *from, jmp_buf jb)
{
	uint32_t id = read_32(from, jb);
	an->genome = genome_pool_get(pool, id);
	if (!an->genome) longjmp(jb, FE_INVALID_GENOME_ID);
	genome_inc(an->genome);
	int dir = fgetc(from);
	if (dir == EOF) longjmp(jb, feof(from) ? FE_UNEXPECTED_EOF : FE_SYSTEM);
	if ((unsigned)dir >= DIRECTION_NUMBER)
		longjmp(jb, FE_INVALID_DIRECTION);
	an->direction = dir;
}

void animal_write(const animal_t *an, FILE *to, jmp_buf jb)
{
	write_32(genome_get_id(an->genome), to, jb);
	if (fputc(an->direction, to) == EOF) longjmp(jb, FE_SYSTEM);
}
