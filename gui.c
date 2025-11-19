#include <ncurses.h>

#define SIZE 3

int selected_row = 0;
int selected_col = 0;
char board[SIZE][SIZE];

void init_gui() {
    initscr();          
    cbreak();           
    noecho();           
    keypad(stdscr, 1);  
    curs_set(0);        

    //initializeaza tabla
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';
}

void draw_board() {
    clear();

    mvprintw(0, 0, "Tic Tac Toe (apasati q pentru iesire)");

    int start_y = 2;
    int start_x = 4;

    //desenre liniile orizontale
    for (int i = 0; i <= SIZE; i++) {
        mvhline(start_y + i * 2, start_x, '-', 13);
    }

    //desenare liniile verticale
    for (int j = 0; j <= SIZE; j++) {
        mvvline(start_y, start_x + j * 4, '|', 6);
    }

    //plasare X și O în interiorul celulelor
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {

            //evidentiere casuta selectata
            if (i == selected_row && j == selected_col)
                attron(A_REVERSE);

            mvprintw(start_y + i * 2 + 1,
                     start_x + j * 4 + 2,
                     "%c", board[i][j]);

            if (i == selected_row && j == selected_col)
                attroff(A_REVERSE);
        }
    }

    refresh();
}

int main() {
    init_gui();
    draw_board();

    int ch;

    while ((ch = getch()) != 'q') {

        switch (ch) {
            case KEY_UP:
                if (selected_row > 0) selected_row--;
                break;

            case KEY_DOWN:
                if (selected_row < SIZE - 1) selected_row++;
                break;

            case KEY_LEFT:
                if (selected_col > 0) selected_col--;
                break;

            case KEY_RIGHT:
                if (selected_col < SIZE - 1) selected_col++;
                break;

            case 'x':
            case 'X':
                board[selected_row][selected_col] = 'X';
                break;

            case 'o':
            case 'O':
                board[selected_row][selected_col] = 'O';
                break;
        }

        draw_board();
    }

    endwin();  // Opreste modul ncurses
    return 0;
}
