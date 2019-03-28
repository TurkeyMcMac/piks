#include "world.h"
#include "util.h"
#include <assert.h>
#include <stdlib.h>

int world_init(world_t *world,
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
	if (!world->cells) return -1;
	return 0;
}

void world_destroy(world_t *world)
{
	genome_pool_destroy(&world->genomes);
	free(world->cells);
}

void world_populate(world_t *world, size_t pop)
{
	scatter_t scat; 
	if (pop == (size_t)-1) pop = genome_pool_get_count(&world->genomes);
	scatter_init(&scat, world->width * world->height, pop);
	while (scatter_has_next(&scat)) {
		size_t i = scatter_next(&scat, &world->rand);
		genome_t *gnm = genome_random(&world->genomes, &world->rand);
		direction_t dir = next_random(&world->rand) % DIRECTION_NUMBER;
		animal_init(&world->cells[i], gnm, dir);
	}
}

animal_t *world_get(world_t *world, size_t x, size_t y)
{
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
	assert(!"Unreachable");
	return -1;
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

static void create_baby(world_t *world, animal_t *parent, animal_t *child)
{
	if (next_random(&world->rand) % 100 < MUTATION_CHANCE) {
		animal_mutant(parent, child, &world->rand);
	} else {
		animal_clone(parent, child);
	}
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
		if (animal_is_null(targ)) {
			*targ = *an;
			animal_null(an);
		}
		break;
	case UA_BABY:
		place = get_wrapped(world,
			x, y, WRAPPING2_GET(flags), (dir + 2) & 0x3);
		if (animal_is_null(place) && !animal_is_null(targ)) {
			animal_die(targ);
			create_baby(world, an, place);
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

void world_read(world_t *world, FILE *from, jmp_buf jb)
{
	jmp_buf local;
	file_error_t err = FE_SUCCESS;
	read_file_header(from, jb);
	world->width = read_32(from, jb);
	world->height = read_32(from, jb);
	if (world->width < 2 || world->height < 2)
		longjmp(jb, FE_INVALID_DIMENSIONS);
	world->rand = read_32(from, jb);
	genome_pool_read(&world->genomes, from, jb);
	world->cells = calloc(world->width * world->height,
		sizeof(*world->cells));
	if (!world->cells) longjmp(jb, FE_SYSTEM);
	if ((err = setjmp(local))) goto error_cells;
	for (size_t i = 0; i < world->width * world->height; ++i) {
		int byte = fgetc(from);
		if (byte == EOF) {
			if (feof(from)) break;
			err = FE_SYSTEM;
			goto error_cells;
		}
		if (byte & 0x80) {
			animal_read(&world->cells[i], &world->genomes, from,
				local);
		} else {
			i += byte;
		}
	}
	return;

error_cells:
	free(world->cells);
	longjmp(jb, err);
}

void world_write(const world_t *world, FILE *to, jmp_buf jb)
{
	write_file_header(to, jb);
	write_32(world->width, to, jb);
	write_32(world->height, to, jb);
	write_32(world->rand, to, jb);
	genome_pool_write(&world->genomes, to, jb);
	int null_streak = -1;
	for (size_t i = 0; i < world->width * world->height; ++i) {
		bool null = animal_is_null(&world->cells[i]);
		if ((!null && null_streak >= 0) || null_streak >= 127) {
			if (fputc(null_streak, to) == EOF)
				longjmp(jb, FE_SYSTEM);
			null_streak = -1;
		}
		if (null) {
			++null_streak;
		} else {
			if (fputc(0x80, to) == EOF)
				longjmp(jb, FE_SYSTEM);
			animal_write(&world->cells[i], to, jb);
		}
	}
}
