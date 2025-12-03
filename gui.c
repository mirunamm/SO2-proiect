#include <ncurses.h>
#include <stdlib.h>
#include "client.h"

#define SIZE 3

int selected_row = 0;
int selected_col = 0;
char board[SIZE][SIZE];

void sync_board() {
    for (int i = 0; i < 9; i++)
        board[i/3][i%3] = tabla[i];
}

void draw_board() {
    clear();

    mvprintw(0, 0, "Tic Tac Toe (q pentru iesire)");

    int start_y = 2;
    int start_x = 4;

    for (int i = 0; i <= SIZE; i++)
        mvhline(start_y + i * 2, start_x, '-', 13);

    for (int j = 0; j <= SIZE; j++)
        mvvline(start_y, start_x + j * 4, '|', 6);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (i == selected_row && j == selected_col)
                attron(A_REVERSE);

            mvprintw(start_y + i * 2 + 1, start_x + j * 4 + 2, "%c", board[i][j]);

            if (i == selected_row && j == selected_col)
                attroff(A_REVERSE);
        }

    refresh();
}

int handle_enter() {
    int poz = selected_row * 3 + selected_col;

    if (tabla[poz] != ' ')
        return 0; // ocupata

    // muta X
    tabla[poz] = 'X';
    board[selected_row][selected_col] = 'X';

    draw_board();

    // verifica daca X a castigat
    int w = check_win();
    if (w == 1) {
        mvprintw(15, 0, "X a castigat!");
        refresh();
        return 1; // GUI trebuie sa se opreasca
    }

    // muta adversarul (O)
    int adv = find_first_free();
    if (adv == -1) {
        mvprintw(15, 0, "Remiza!");
        refresh();
        return 1;
    }

    tabla[adv] = 'O';
    board[adv/3][adv%3] = 'O';

    draw_board();

    // verifica daca O a castigat
    w = check_win();
    if (w == 2) {
        mvprintw(15, 0, "O a castigat!");
        refresh();
        return 1;
    }

    return 0; // jocul continua
}


int main() {
    init_tabla();
    sync_board();

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);

    draw_board();

    int ch;
    int stop = 0;

    while (!stop && (ch = getch()) != 'q') {

        switch (ch) {
            case KEY_UP:    if (selected_row > 0) selected_row--; break;
            case KEY_DOWN:  if (selected_row < SIZE-1) selected_row++; break;
            case KEY_LEFT:  if (selected_col > 0) selected_col--; break;
            case KEY_RIGHT: if (selected_col < SIZE-1) selected_col++; break;
            case 10: stop = handle_enter(); break;
        }
    
        draw_board();
    }
    
    mvprintw(17, 0, "Apasa orice tasta pentru a iesi...");
    refresh();
    getch();
    
    endwin();
    return 0;
    
    
}
