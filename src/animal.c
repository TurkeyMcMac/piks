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
	*src = *dst;
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

void animal_mutant(animal_t *src, animal_t *dst)
{
	genome_t *clone = genome_clone(src->genome);
	genome_inc(clone);
	animal_init(dst, clone, src->direction);
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
