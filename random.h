#ifndef RANDOM_INCLUDED_
#define RANDOM_INCLUDED_

typedef uint32_t rand_t;

rand_t randomize(rand_t seed);

static inline rand_t next_random(rand_t *randp)
{
	return *randp = randomize(*randp);
}

#endif /* RANDOM_INCLUDED_ */
