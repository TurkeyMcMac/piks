#include "animal.h"
#include <assert.h>

void animal_init(animal_t *an, genome_t *gnm, direction_t dir)
{
	genome_inc(gnm);
	an->genome = (uintptr_t)gnm;
	assert((an->genome & 0x3) == 0);
	an->genome |= dir;
}

void animal_clone(animal_t *src, animal_t *dst)
{
	*src = *dst;
	genome_inc((genome_t *)(dst->genome & ~0x3));
}

void animal_null(animal_t *an)
{
	an->genome = 0;
}

bool animal_is_null(const animal_t *an)
{
	return an->genome == 0;
}

void animal_mutant(animal_t *src, animal_t *dst)
{
	genome_t *clone = genome_clone((genome_t *)(src->genome & ~0x3));
	genome_inc(clone);
	animal_init(dst, clone, src->genome & 0x3);
}

action_t animal_get_action(const animal_t *an, input_t input)
{
	return genome_get((const genome_t *)(an->genome & ~0x3), input);
}

void animal_turn(animal_t *an, int quarts)
{
	an->genome = (an->genome & ~0x3) | ((an->genome + quarts) & 0x3);
}

direction_t animal_get_direction(const animal_t *an)
{
	return an->genome & 0x3;
}

void animal_die(animal_t *an)
{
	genome_dec((genome_t *)(an->genome & ~0x3));
}
