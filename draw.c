#include "draw.h"
#include "board.h"

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
	clear();
	refresh();

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

static void draw_border(WINDOW *win, Board *b)
{
      wattrset(win, COLOR_NORMAL);
      int len = b->size * 2;
      mvwvline(win, 1, len, ACS_VLINE, len-1);
      mvwhline(win, len, 0, ACS_HLINE, len);
      mvwaddch(win, len, len, ACS_LRCORNER);
}

static void draw_bit(WINDOW *win, int y, int x, int bit)
{
	wattrset(win, bit ? COLOR_BIT_ON : COLOR_BIT_OFF);
	mvwaddch(win, y*2 + 1, x*2 + 1, bit + '0');
}

static void draw_bits(WINDOW *win, Board *b)
{
	for (int y = 0; y < b->size; y++) {
		for (int x = 0; x < b->size; x++) {
			draw_bit(win, y, x, board_bit(b, y, x));
		}
	}
}

static void draw_row_num(WINDOW *win, Board *b, int i)
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

static void draw_row_nums(WINDOW *win, Board *b)
{
	for (int i = 0; i < b->size; i++) {
		draw_row_num(win, b, i);
	}
}

static void draw_col_num(WINDOW *win, Board *b, int i)
{
	Num n = board_col_num(b, i);

	if (n.correct)
		wattrset(win, COLOR_NUM_CORRECT);
	else
		wattrset(win, COLOR_NUM_NORMAL);

	char buf[32];
	int y = b->size * 2 + 1;
	int x = 2 * i + 1;
	snprintf(buf, 32, "%u", n.num);
	char *ch = buf;
	while (*ch) {
		mvwaddch(win, y, x, *ch);
		ch++;
		y++;
	}
}

static void draw_col_nums(WINDOW *win, Board *b) {
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
	draw_col_nums(win, b);
	set_cursor_position(win, cursor);
	wrefresh(win);
}
