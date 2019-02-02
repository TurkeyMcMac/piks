#include "random.h"
#include "util.h"

rand_t randomize(rand_t seed)
{
	static const rand_t rand_table[16] = {
		17, 11, 4, 15, 14, 2, 7, 1, 13, 11, 13, 14, 12, 15, 19, 5
	};
	return rotright32(seed, seed) + rand_table[seed % 16];
}

void scatter_init(scatter_t *scat, size_t limit, size_t count)
{
	if (count == 0) {
		scat->limit = 0;
	} else {
		scat->limit = limit;
		scat->division = limit / count;
	}
	scat->i = 0;
}

bool scatter_has_next(const scatter_t *scat)
{
	return scat->i < scat->limit;
}

size_t scatter_next(scatter_t *scat, rand_t *seed)
{
	size_t next = scat->i + scat->division % next_random(seed);
	scat->i += scat->division;
	return next;
}
