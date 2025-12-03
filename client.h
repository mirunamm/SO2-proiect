#ifndef CLIENT_H
#define CLIENT_H

extern char tabla[9];

void init_tabla();
int client_move(int poz);
int check_win();
int find_first_free();
int board_full();
void draw_board();
#endif
