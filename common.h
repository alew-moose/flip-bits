#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t u32;

typedef struct {
	u32 num;
	bool correct;
} Num;

typedef struct {
	int y, x;
} Coord;

typedef enum {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
} Dir;

#endif
