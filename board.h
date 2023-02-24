#ifndef BOARD_H
#define BOARD_H

#include "common.h"

typedef struct {
	int size;
  u32 *rows_have;
  u32 *cols_have;
  u32 *rows_want;
  u32 *cols_want;
} Board;

Board *rand_board(int size);
void free_board(Board *b);
int board_bit(Board *b, int y, int x);
bool board_correct(Board *b);
void board_flip_bit(Board *b, int y, int x);
Num board_row_num(Board *b, int y);
Num board_col_num(Board *b, int x);
Coord cursor_move(Coord cursor, Dir dir, int size);

#endif
