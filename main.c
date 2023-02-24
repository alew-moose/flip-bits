#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <ncurses.h>
#include "board.h"
#include "draw.h"

enum {NO_CTRL, CTRL, ALT};

enum {
	KEY_CTRL_UP,
	KEY_CTRL_DOWN,
	KEY_CTRL_LEFT,
	KEY_CTRL_RIGHT,
	KEY_ALT_UP,
	KEY_ALT_DOWN,
	KEY_ALT_LEFT,
	KEY_ALT_RIGHT,
};

int read_input(void);

int input(int ctrl, int prefix[], int prefix_len)
{
	int ch = getch();

	if (ctrl == CTRL) {
		switch (ch) {
		case 65: return KEY_CTRL_UP;
		case 66: return KEY_CTRL_DOWN;
		case 68: return KEY_CTRL_LEFT;
		case 67: return KEY_CTRL_RIGHT;
		default: return read_input();
		}
	}
	if (ctrl == ALT) {
		switch (ch) {
		case 65: return KEY_ALT_UP;
		case 66: return KEY_ALT_DOWN;
		case 68: return KEY_ALT_LEFT;
		case 67: return KEY_ALT_RIGHT;
		default: return read_input();
		}
	}

	if (prefix_len == 0) {
		switch (ch) {
		case 53: return input(CTRL, NULL, 0);
		case 51: return input(ALT, NULL, 0);
		default: return input(NO_CTRL, NULL, 0);
		}
	}

	if (ch == prefix[0]) {
		return input(ctrl, &prefix[1], prefix_len-1);
	}

	switch (ch) {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
	case ' ':
	case 'q': case 'Q':
		return ch;
	default:
		return input(ctrl, prefix, prefix_len);
	}

	return 0;
}

int read_input(void) {
	int ctrl_prefix[] = {27, 91, 49, 59};
	int ctrl_prefix_len = 4;
	return input(NO_CTRL, ctrl_prefix, ctrl_prefix_len);
}

void flip_multi(Board *b, Coord c, int dy, int dx)
{
	int y = c.y;
	int x = c.x;
	while (y >= 0 && y < b->size && x >= 0 && x < b->size) {
		board_flip_bit(b, y, x);
		y += dy;
		x += dx;
	}
}

int main(void)
{
	srand(0);
	Board *b = rand_board(6);
	if (!b) {
		return 1;
	}

	if (ui_setup() < 0) {
		endwin();
		return 1;
	}

	/* clear(); */

	WINDOW *win = window_init(b->size);
	if (!win) {
		// TODO
		endwin();
		return 1;
	}
	Coord cursor = (Coord){0, 0};

	/* refresh(); */
	window_draw(win, b, cursor);

	/* mvwprintw(win, 0, 0, "aosehtn"); */
	/* wrefresh(win); */

	int ch;
	while (1) {
		Dir dir;
		ch = read_input();
		/* mvwprintw(win, 0, 0, "%d      ", ch); */
		/* wrefresh(win); */

		switch (ch) {
		case KEY_ALT_UP:     cursor.y = 0; break;
		case KEY_ALT_DOWN:   cursor.y = b->size - 1; break;
		case KEY_ALT_LEFT:   cursor.x = 0; break;
		case KEY_ALT_RIGHT:  cursor.x = b->size - 1; break;
		case KEY_CTRL_UP:    flip_multi(b, cursor, -1, 0); break;
		case KEY_CTRL_DOWN:  flip_multi(b, cursor, 1, 0); break;
		case KEY_CTRL_LEFT:  flip_multi(b, cursor, 0, -1); break;
		case KEY_CTRL_RIGHT: flip_multi(b, cursor, 0, 1); break;

		default: goto NOT_CTRL;
		}
		wclear(win);
		window_draw(win, b, cursor);
			if (board_correct(b)) {
				sleep(1);
				free_board(b);
				b = rand_board(6);
				cursor = (Coord){0, 0};
				wclear(win);
				window_draw(win, b, cursor);
			}
		continue;

	NOT_CTRL:

		switch (ch) {
		case 'q': case 'Q': goto EXIT; break;
		case KEY_UP:    dir = DIR_UP; break;
		case KEY_DOWN:  dir = DIR_DOWN; break;
		case KEY_LEFT:  dir = DIR_LEFT; break;
		case KEY_RIGHT: dir = DIR_RIGHT; break;
		case ' ':
			board_flip_bit(b, cursor.y, cursor.x);
			window_draw(win, b, cursor);
			if (board_correct(b)) {
				sleep(1);
				free_board(b);
				b = rand_board(6);
				cursor = (Coord){0, 0};
				wclear(win);
				window_draw(win, b, cursor);
			}
			continue;
			break;
		default: continue;
		}
		cursor = cursor_move(cursor, dir, b->size);
		window_draw(win, b, cursor);
	}


EXIT:
	ui_shutdown();
	return 0;
}


