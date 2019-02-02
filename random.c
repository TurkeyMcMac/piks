#include "random.h"
#include "util.h"

rand_t randomize(rand_t seed)
{
	static const rand_t rand_table[16] = {
		17, 11, 4, 15, 14, 2, 7, 1, 13, 11, 13, 14, 12, 15, 19, 5
	};
	return rotright32(seed, seed) + rand_table[seed % 16];
}
