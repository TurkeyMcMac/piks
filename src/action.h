#ifndef ACTION_INCLUDED_
#define ACTION_INCLUDED_

#include <stdint.h>

typedef uint8_t input_t;

typedef enum {
	ACTION_MOVE,
	ACTION_LEFT,
	ACTION_RIGHT,
	ACTION_BABY

#define ACTION_NUMBER 4
} action_t;

#endif /* ACTION_INCLUDED_ */
