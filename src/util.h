#ifndef UTIL_INCLUDED_
#define UTIL_INCLUDED_

#include <stdint.h>

#define ROTRIGHT_FUN(bits) \
	static inline \
	uint##bits##_t rotright##bits(uint##bits##_t num, signed amt) \
	{ \
		unsigned amount = (unsigned)amt % bits;	\
		if (amount) { \
			return (num >> amount) | (num << (bits - amount)); \
		} else { \
			/* If amount == 0, num << (bits - amount) is undefined. */ \
			return num; \
		} \
	} struct rotright##bits##_requires_semicolon_

ROTRIGHT_FUN(8);
ROTRIGHT_FUN(32);

#undef ROTRIGHT_FUN

#endif /* UTIL_INCLUDED_ */
