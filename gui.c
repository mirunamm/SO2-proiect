#include <ncurses.h>
#include "gui.h"

#define SIZE 3

static int selected_row = 0;
static int selected_col = 0;
static char board[SIZE][SIZE];

void gui_init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);
}

void gui_close() {
    endwin();
}

void gui_draw_board(char *tabla) {
    for (int i = 0; i < 9; i++)
        board[i/3][i%3] = tabla[i];

    clear();
    mvprintw(0, 0, "Tic Tac Toe");

    int y = 2, x = 4;

    for (int i = 0; i <= SIZE; i++)
        mvhline(y + i * 2, x, '-', 13);
    for (int j = 0; j <= SIZE; j++)
        mvvline(y, x + j * 4, '|', 6);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (i == selected_row && j == selected_col)
                attron(A_REVERSE);

            mvprintw(y + i * 2 + 1, x + j * 4 + 2, "%c", board[i][j]);

            if (i == selected_row && j == selected_col)
                attroff(A_REVERSE);
        }

    refresh();
}

int gui_get_move(int your_turn) {
    if (!your_turn) return -1;

    int ch;
    while ((ch = getch())) {
        switch (ch) {
            case KEY_UP:    if (selected_row > 0) selected_row--; break;
            case KEY_DOWN:  if (selected_row < 2) selected_row++; break;
            case KEY_LEFT:  if (selected_col > 0) selected_col--; break;
            case KEY_RIGHT: if (selected_col < 2) selected_col++; break;
            case 10: return selected_row * 3 + selected_col;
        }
        gui_draw_board((char *)board);
    }
    return -1;
}
