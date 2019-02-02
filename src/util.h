#ifndef UTIL_INCLUDED_
#define UTIL_INCLUDED_

#include <stdint.h>

#define ROTRIGHT_FUN(bits) \
	static inline \
	uint##bits##_t rotright##bits(uint##bits##_t num, signed amt) \
	{ \
		unsigned amount = (unsigned)amt % bits;	\
		return (num >> amount) | (num << (bits - amount)); \
	} struct rotright##bits##_requires_semicolon_

ROTRIGHT_FUN(8);
ROTRIGHT_FUN(32);

#undef ROTRIGHT_FUN

#endif /* UTIL_INCLUDED_ */
