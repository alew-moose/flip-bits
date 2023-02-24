#include <stdlib.h>
#include <math.h>
#include "board.h"

Coord cursor_move(Coord cursor, Dir dir, int size)
{
	Coord c = cursor;
	switch (dir) {
	case DIR_UP: c.y--; break;
	case DIR_DOWN: c.y++; break;
	case DIR_LEFT: c.x--; break;
	case DIR_RIGHT: c.x++; break;
	}

	c.y = (size + c.y) % size;
	c.x = (size + c.x) % size;

	return c;

}

static int coord_to_idx(int size, int coord)
{
	return size - coord - 1;
}

static Board *blank_board(int size)
{
	Board *b = malloc(sizeof(Board));
	if (!b)
		return NULL;

	b->rows_have = calloc(size, sizeof(u32));
	if (!b->rows_have)
		goto ERR_ROWS_HAVE;
	b->cols_have = calloc(size, sizeof(u32));
	if (!b->cols_have)
		goto ERR_COLS_HAVE;
	b->rows_want = calloc(size, sizeof(u32));
	if (!b->rows_want)
		goto ERR_ROWS_WANT;
	b->cols_want = calloc(size, sizeof(u32));
	if (!b->cols_want)
		goto ERR_COLS_WANT;
	b->size = size;

	return b;

ERR_COLS_WANT:
	free(b->rows_want);
ERR_ROWS_WANT:
	free(b->cols_have);
ERR_COLS_HAVE:
	free(b->rows_have);
ERR_ROWS_HAVE:
	free(b);

	return NULL;
}

Board *rand_board(int size)
{

	Board *b = blank_board(size);
	if (!b)
		return NULL;

	u32 max = pow(2, size);
	for (int y = 0; y < size; y++) {
		b->rows_want[y] = rand() % max;
	}

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			u32 bit = b->rows_want[y] & (1 << (size - x - 1)) ? 1 : 0;
			b->cols_want[x] |= bit << (size - y - 1);
		}
	}

	return b;
}

void free_board(Board *b)
{
	free(b->rows_have);
	free(b->cols_have);
	free(b->rows_want);
	free(b->cols_want);
	free(b);
}

bool board_correct(Board *b)
{
	for (int y = 0; y < b->size; y++)
		if (b->rows_have[y] != b->rows_want[y])
			return false;
	return true;
}

// TODO: free_board

int board_bit(Board *b, int y, int x)
{
	if (b->rows_have[y] & (1 << coord_to_idx(b->size, x)))
		return 1;
	return 0;
}

void board_flip_bit(Board *b, int y, int x)
{
	b->rows_have[y] ^= 1 << coord_to_idx(b->size, x);
	b->cols_have[x] ^= 1 << coord_to_idx(b->size, y);
}

Num board_row_num(Board *b, int y)
{
	Num n;
	n.num = b->rows_want[y];
	n.correct = n.num == b->rows_have[y];
	return n;
}

Num board_col_num(Board *b, int x)
{
	Num n;
	n.num = b->cols_want[x];
	n.correct = n.num == b->cols_have[x];
	return n;
}
