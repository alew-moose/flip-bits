#ifndef DRAW_H
#define DRAW_H

#include <ncurses.h>
#include "board.h"
#include "common.h"

int ui_setup(void);
int ui_shutdown(void);
WINDOW *window_init(int board_size);
void window_free(WINDOW *win);
void window_draw(WINDOW *win, Board *b, Coord cursor);

#endif
