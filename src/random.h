#ifndef RANDOM_INCLUDED_
#define RANDOM_INCLUDED_

#include <stdbool.h>

typedef uint32_t rand_t;

rand_t randomize(rand_t seed);

typedef struct {
	size_t limit, division, i;
} scatter_t;

static inline rand_t next_random(rand_t *randp)
{
	return *randp = randomize(*randp);
}

void scatter_init(scatter_t *scat, size_t limit, size_t count);

bool scatter_has_next(const scatter_t *scat);

size_t scatter_next(scatter_t *scat, rand_t *seed);

#endif /* RANDOM_INCLUDED_ */
