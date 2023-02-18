#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <ncurses.h>

typedef uint32_t u32;

typedef struct {
	int size;
	u32 *bit_rows;
	u32 *row_nums;
	u32 *col_nums;
} Board;

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

Board *blank_board(int size)
{
	Board *b = malloc(sizeof(Board));
	if (!b)
		return NULL;

	u32 *bit_rows, *row_nums, *col_nums;

	if (!(bit_rows = calloc(size, sizeof(u32))))
		goto ERR_BIT_ROWS;
	if (!(row_nums = calloc(size, sizeof(u32))))
		goto ERR_ROW_NUMS;
	if (!(col_nums = calloc(size, sizeof(u32))))
		goto ERR_COL_NUMS;

	b->size = size;
	b->bit_rows = bit_rows;
	b->row_nums = row_nums;
	b->col_nums = col_nums;
	return b;

ERR_COL_NUMS:
	free(row_nums);
ERR_ROW_NUMS:
	free(bit_rows);
ERR_BIT_ROWS:
	free(b);
	return NULL;
}

void board_flip_bit(Board *b, int y, int x)
{
	b->bit_rows[y] ^= 1 << (b->size - x - 1);
}

Board *rand_board(int size)
{

	Board *b = blank_board(size);
	if (!b)
		return NULL;

	u32 maxnum = pow(2.0, size);
	for (int y = 0; y < size; y++) {
		u32 num = rand() % maxnum;
		b->row_nums[y] = num;
	}

	// TODO: col_nums

	return b;

	/* int coords_len = size * size; */
	/* Coord *coords = malloc(coords_len * sizeof(Coord)); */
	/* if (!coords) */
	/* 	goto ERR_COORDS; */

	/* for (int y = 0; y < size; y++) { */
	/* 	for (int x = 0; x < size; x++) { */
	/* 		coords[y * size + x] = (Coord){y, x}; */
	/* 	} */
	/* } */

	/* int to_fill = size * size / 2; // TODO */
	/* int filled = 0; */

	/* while (filled < to_fill) { */
	/* 	int c = rand() % coords_len; */
	/* 	board_flip_bit(b, coords[c].y, coords[c].x); */
	/* 	coords[c] = coords[coords_len - 1]; */
	/* 	coords_len--; */
	/* 	filled++; */
	/* } */

	/* for (int y = 0; y < size; y++) { */
	/* 	b->row_nums[y] = b->bit_rows[y]; */
	/* } */
	/* /1* for (int x = 0; x < size; x++) { *1/ */
	/* /1* 	b->col_nums *1/ */
	/* /1* } *1/ */

	/* free(coords); */

	/* return b; */

/* ERR_COORDS: */
/* 	free(b); */

/* 	return NULL; */
}

int board_bit(Board *b, int y, int x)
{
	if (b->bit_rows[y] & (1 << (b->size - x - 1)))
		return 1;
	return 0;
}

Num board_row_num(Board *b, int y)
{
	Num n;
	n.num = b->row_nums[y];
	n.correct = n.num == b->bit_rows[y];
	return n;
}

// does't work
Num board_col_num(Board *b, int x)
{
	u32 n = 0;
	for (int y = 0; y < b->size; y++) {
		n |= (b->bit_rows[y] & (1 << x));
	}
	Num num;
	num.num = n;
	num.correct = num.num == b->col_nums[x];
	return num;
}

#define COLOR_NORMAL COLOR_PAIR(1)
#define COLOR_BIT_ON (COLOR_PAIR(1) | A_BOLD)
#define COLOR_BIT_OFF (COLOR_PAIR(1))
#define COLOR_NUM_CORRECT (COLOR_PAIR(2) | A_BOLD)
#define COLOR_NUM_NORMAL (COLOR_PAIR(1) | A_BOLD)


int ui_setup(void)
{
	initscr();

	if ((start_color() != OK) ||
	    (noecho() != OK) ||
	    (cbreak() != OK) ||
	    (set_escdelay(0) != OK) ||
	    (keypad(stdscr, true) != OK) ||
	    (init_pair(1, COLOR_WHITE, COLOR_BLACK) != OK) ||
	    (init_pair(2, COLOR_GREEN, COLOR_BLACK) != OK)
	) {
		endwin();
		return -1;
	}

	return 0;
}

int ui_shutdown(void)
{
	return endwin();
}

WINDOW *window_init(int board_size)
{
	int scr_height, scr_width;
	getmaxyx(stdscr, scr_height, scr_width);

	int win_width  = board_size * 2 + 4;
	int win_height = board_size * 2 + 4;
	int win_left   = (scr_width  - win_width)  / 2;
	int win_top    = (scr_height - win_height) / 2;

	WINDOW *win = newwin(win_height, win_width, win_top, win_left);
	if (!win)
		return NULL;

	if (keypad(win, true) != OK) {
		delwin(win);
		return NULL;
	}

	return win;
}

void window_free(WINDOW *win)
{
	delwin(win);
}

void draw_border(WINDOW *win, Board *b)
{
      wattrset(win, COLOR_NORMAL);
      int len = b->size * 2;
      mvwvline(win, 1, len, ACS_VLINE, len-1);
      mvwhline(win, len, 0, ACS_HLINE, len);
      mvwaddch(win, len, len, ACS_LRCORNER);
      wrefresh(win);
}

void draw_bit(WINDOW *win, int y, int x, int bit)
{
	wattrset(win, bit ? COLOR_BIT_ON : COLOR_BIT_OFF);
	mvwaddch(win, y*2 + 1, x*2 + 1, bit + '0');
}

void draw_bits(WINDOW *win, Board *b)
{
	for (int y = 0; y < b->size; y++) {
		for (int x = 0; x < b->size; x++) {
			draw_bit(win, y, x, board_bit(b, y, x));
		}
	}
}

void draw_row_num(WINDOW *win, Board *b, int i)
{
	Num n = board_row_num(b, i);
	if (n.correct)
		wattrset(win, COLOR_NUM_CORRECT);
	else
		wattrset(win, COLOR_NUM_NORMAL);
	int y = 2 * i + 1;
	int x = b->size * 2 + 1;
	mvwprintw(win, y, x, "%d", n.num);
}

void draw_row_nums(WINDOW *win, Board *b)
{
	for (int i = 0; i < b->size; i++) {
		draw_row_num(win, b, i);
	}
}

// XXX: doesn't work
void draw_col_num(WINDOW *win, Board *b, int i)
{
	Num n = board_col_num(b, i);

	if (n.correct)
		wattrset(win, COLOR_NUM_CORRECT);
	else
		wattrset(win, COLOR_NUM_NORMAL);

	int y = 2 * i + 1;
	int x = b->size * 2 + 1 + 10;
	mvwprintw(win, y, x, "%d", n.num);

	/* char buf[32]; */
	/* int y = b->size * 2 + 1; */
	/* int x = 2 * i + 1; */
	/* snprintf(buf, 32, "%u", n.num); */
	/* char *ch = buf; */
	/* while (*ch) { */
	/* 	mvwaddch(win, y, x, *ch); */
	/* 	ch++; */
	/* 	y++; */
	/* } */
}

void draw_col_nums(WINDOW *win, Board *b) {
	for (int i = 0; i < b->size; i++) {
		draw_col_num(win, b, i);
	}
}

void set_cursor_position(WINDOW *win, Coord cursor)
{
	int y = cursor.y * 2 + 1;
	int x = cursor.x * 2 + 1;
	wmove(win, y, x);
}

void window_draw(WINDOW *win, Board *b, Coord cursor)
{
	draw_border(win, b);
	draw_bits(win, b);
	draw_row_nums(win, b);
	/* draw_col_nums(win, b); */
	set_cursor_position(win, cursor);
	wrefresh(win);
}

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

int main(void)
{
	srand(time(NULL));
	Board *b = rand_board(6);
	(void)b;

	clear();

	if (ui_setup() < 0) {
		endwin();
		return 1;
	}
	WINDOW *win = window_init(b->size);
	if (!win) {
		// TODO
		return 1;
	}
	Coord cursor = (Coord){0, 0};
	window_draw(win, b, cursor);


	int ch;
	while ((ch = getch())) {
		window_draw(win, b, cursor);
		Dir dir = DIR_UP;
		switch (ch) {
		case KEY_UP:    dir = DIR_UP;    break;
		case KEY_DOWN:  dir = DIR_DOWN;  break;
		case KEY_LEFT:  dir = DIR_LEFT;  break;
		case KEY_RIGHT: dir = DIR_RIGHT; break;
		case ' ':
			board_flip_bit(b, cursor.y, cursor.x);
			window_draw(win, b, cursor);
			continue;
			break;
		default: continue;
		}
		cursor = cursor_move(cursor, dir, b->size);
		window_draw(win, b, cursor);
	}


	ui_shutdown();
	return 0;
}


