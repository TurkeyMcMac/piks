#include "world.h"
#include "util.h"
#include <assert.h>
#include <stdlib.h>

void world_init(world_t *world,
	size_t width,
	size_t height,
	size_t genomes,
	rand_t seed)
{
	genome_pool_init(&world->genomes, genomes);
	world->width = width;
	world->height = height;
	world->rand = seed;
	world->cells = calloc(width * height, sizeof(*world->cells));
}

void world_destroy(world_t *world)
{
	genome_pool_destroy(&world->genomes);
	free(world->cells);
}

void world_populate(world_t *world, size_t pop)
{
	scatter_t scat; 
	if (pop == -1) pop = genome_pool_get_count(&world->genomes);
	scatter_init(&scat, world->width * world->height, pop);
	while (scatter_has_next(&scat)) {
		size_t i = scatter_next(&scat, &world->rand);
		genome_t *gnm = genome_random(&world->genomes, &world->rand);
		direction_t dir = next_random(&world->rand) % DIRECTION_NUMBER;
		animal_init(&world->cells[i], gnm, dir);
	}
	size_t j = 0;
	for (size_t x = 0; x < world->width; ++x) {
		for (size_t y = 0; y < world->height; ++y) {
			if (!animal_is_null(world_get(world, x, y))) ++j;
		}
	}
	assert(j == pop);
}

animal_t *world_get(world_t *world, size_t x, size_t y)
{
	//printf("Get (%zu, %zu)\n", x, y);
	assert(x < world->width);
	assert(y < world->height);
	return &world->cells[y * world->width + x];
}

/* Animal flags format:
 * 0WWW0000MWWW00TT
 * M = moved this step?
 * W = wrapping of movement
 * T = type of action (none, movement, baby)
 */

#define MOVED 0x0080
#define WRAPPING_GET(flags) (((flags) & 0x0070) >> 4)
#define WRAPPING_SET(flags, w) ((flags) |= (w) << 4)
#define WRAPPING2_GET(flags) (((flags) & 0x7000) >> 12)
#define WRAPPING2_SET(flags, w) ((flags) |= (w) << 12)
#define TYPE_GET(flags) ((flags) & 0x03)
#define TYPE_SET(flags, t) ((flags) |= (t))

enum wrapping {
	WRAP_NONE = 0,
	WRAP_RIGHT,
	WRAP_TOP,
	WRAP_LEFT,
	WRAP_BOTTOM
};

enum update_action { UA_NONE = 0, UA_MOVE, UA_BABY };


static enum wrapping get_wrapping(direction_t dir,
	enum wrapping r, enum wrapping t, enum wrapping l, enum wrapping b)
{
	switch (dir) {
	case DIRECTION_RIGHT: return r;
	case DIRECTION_UP: return t;
	case DIRECTION_LEFT: return l;
	case DIRECTION_DOWN: return b;
	}
}

static void get_action(world_t *world, size_t x, size_t y,
	enum wrapping r, enum wrapping t, enum wrapping l, enum wrapping b)
{
	animal_t *animal = world_get(world, x, y);
	uint16_t *flags = animal_flags(animal);
	if (animal_is_null(animal)) return;
	size_t xp1 = r ? 0 : x + 1;
	size_t yp1 = b ? 0 : y + 1;
	size_t xm1 = l ? world->width - 1 : x - 1;
	size_t ym1 = t ? world->height - 1 : y - 1;
#	define GETBIT(x, y) \
		(input_t)(animal_is_null(world_get(world, (x), (y))))
	input_t input =
		GETBIT(xp1,   y) << 0 |
		GETBIT(xp1, ym1) << 1 |
		GETBIT(  x, ym1) << 2 |
		GETBIT(xm1, ym1) << 3 |
		GETBIT(xm1,   y) << 4 |
		GETBIT(xm1, yp1) << 5 |
		GETBIT(  x, yp1) << 6 |
		GETBIT(xp1, yp1) << 7 ;
#	undef GETBIT
	direction_t dir = animal_get_direction(animal);
	input = rotright8(input, dir * 2);
	action_t action = animal_get_action(animal, input);
	*flags = 0;
	switch (action) {
	case ACTION_MOVE:
		TYPE_SET(*flags, UA_MOVE);
		break;
	case ACTION_BABY:
		TYPE_SET(*flags, UA_BABY);
		WRAPPING2_SET(*flags,
			get_wrapping((dir + 2) & 0x3, r, t, l, b));
		break;
	case ACTION_RIGHT:
		animal_turn(animal, -1);
		break;
	case ACTION_LEFT:
		animal_turn(animal, 1);
		break;
	}
	WRAPPING_SET(*flags, get_wrapping(dir, r, t, l, b));
}

static void get_actions(world_t *world)
{
	size_t x_max = world->width - 1, y_max = world->height - 1;
	/* Top left: */
	get_action(world, 0, 0,
		WRAP_NONE, WRAP_TOP, WRAP_LEFT, WRAP_NONE);
	/* Top row: */
	for (size_t x = 1; x < x_max; ++x) {
		get_action(world, x, 0,
			WRAP_NONE, WRAP_TOP, WRAP_NONE, WRAP_NONE);
	}
	/* Top right: */
	get_action(world, x_max, 0,
		WRAP_RIGHT, WRAP_TOP, WRAP_NONE, WRAP_NONE);
	/* Left column: */
	for (size_t y = 1; y < y_max; ++y) {
		get_action(world, 0, y,
			WRAP_NONE, WRAP_NONE, WRAP_LEFT, WRAP_NONE);
	}
	/* Center: */
	for (size_t x = 1; x < x_max; ++x) {
		for (size_t y = 1; y < y_max; ++y) {
			get_action(world, x, y,
				WRAP_NONE, WRAP_NONE, WRAP_NONE, WRAP_NONE);
		}
	}
	/* Right column: */
	for (size_t y = 1; y < y_max; ++y) {
		get_action(world, x_max, y,
			WRAP_RIGHT, WRAP_NONE, WRAP_NONE, WRAP_NONE);
	}
	/* Bottom left: */
	get_action(world, 0, y_max,
		WRAP_NONE, WRAP_NONE, WRAP_LEFT, WRAP_BOTTOM);
	/* Bottom row: */
	for (size_t x = 1; x < x_max; ++x) {
		get_action(world, x, y_max,
			WRAP_NONE, WRAP_NONE, WRAP_NONE, WRAP_BOTTOM);
	}
	/* Bottom right: */
	get_action(world, x_max, y_max,
		WRAP_RIGHT, WRAP_NONE, WRAP_NONE, WRAP_BOTTOM);
}

static animal_t *get_wrapped(world_t *world,
	size_t x,
	size_t y,
	enum wrapping wrap,
	direction_t dir)
{
	size_t targ_x = x, targ_y = y;
	switch (wrap) {
	case WRAP_RIGHT: targ_x = 0; break;
	case WRAP_TOP: targ_y = world->height - 1; break;
	case WRAP_LEFT: targ_x = world->width - 1; break;
	case WRAP_BOTTOM: targ_y = 0; break;
	case WRAP_NONE:
		switch (dir) {
		case DIRECTION_RIGHT: ++targ_x; break;
		case DIRECTION_UP: --targ_y; break;
		case DIRECTION_LEFT: --targ_x; break;
		case DIRECTION_DOWN: ++targ_y; break;
		}
		break;
	}
	return world_get(world, targ_x, targ_y);
}

static void carry_out_action(world_t *world, size_t x, size_t y)
{
	animal_t *an = world_get(world, x, y);
	uint16_t flags = *animal_flags(an);
	if (animal_is_null(an) || TYPE_GET(flags) == UA_NONE || flags & MOVED)
		return;
	if (flags & MOVED) {
		*animal_flags(an) &= ~MOVED;
		return;
	}
	direction_t dir = animal_get_direction(an);
	animal_t *targ, *place;
	targ = get_wrapped(world, x, y, WRAPPING_GET(flags), dir);
	switch (TYPE_GET(flags)) {
	case UA_MOVE:
		{
			animal_t temp;
			temp = *targ;
			*targ = *an;
			*an = temp;
		} break;
	case UA_BABY:
		place = get_wrapped(world,
			x, y, WRAPPING2_GET(flags), (dir + 2) & 0x3);
		if (animal_is_null(place) && !animal_is_null(targ)) {
			animal_die(targ);
			animal_clone(an, place); /* TODO: evolution */
			animal_turn(place, 2);
			*animal_flags(place) |= MOVED;
		}
		break;
	default: return;
	}
	*animal_flags(targ) |= MOVED;

}

static void carry_out_actions(world_t *world)
{
	for (size_t x = 0; x < world->width; ++x) {
		for (size_t y = 0; y < world->height; ++y) {
			carry_out_action(world, x, y);
		}
	}
}

void world_step(world_t *world)
{
	get_actions(world);
	carry_out_actions(world);
}
