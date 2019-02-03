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
	genome_inc(mut);
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

uint8_t *animal_flags(animal_t *an)
{
	return &an->flags;
}

void animal_die(animal_t *an)
{
	genome_dec(an->genome);
	animal_null(an);
}
