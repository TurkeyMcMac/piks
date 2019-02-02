#ifndef ANIMALS_INCLUDED_
#define ANIMALS_INCLUDED_

#include "genome.h"
#include <stddef.h>

typedef enum {
	DIRECTION_RIGHT,
	DIRECTION_UP,
	DIRECTION_LEFT,
	DIRECTION_DOWN,

	DIRECTION_NUMBER
} direction_t;

typedef struct {
	uintptr_t genome;
} animal_t;

void animal_init(animal_t *an, genome_t *gnm, direction_t dir);

void animal_clone(animal_t *src, animal_t *dst);

void animal_mutant(animal_t *src, animal_t *dst);

action_t animal_get_action(const animal_t *an, input_t input);

void animal_turn(animal_t *an, int quarts);

direction_t animal_get_direction(const animal *an);

void animal_die(animal_t *an);

#endif /* ANIMALS_INCLUDED_ */
