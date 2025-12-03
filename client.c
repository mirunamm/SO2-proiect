#include <stdio.h>

char tabla[9];

void init_tabla() {
    for (int i = 0; i < 9; i++)
        tabla[i] = ' ';
}

int check_win() {
    // linii
    for (int i = 0; i < 3; i++) {
        if (tabla[i*3] != ' ' &&
            tabla[i*3] == tabla[i*3+1] &&
            tabla[i*3] == tabla[i*3+2])
            return (tabla[i*3] == 'X') ? 1 : 2;
    }

    // coloane
    for (int i = 0; i < 3; i++) {
        if (tabla[i] != ' ' &&
            tabla[i] == tabla[i+3] &&
            tabla[i] == tabla[i+6])
            return (tabla[i] == 'X') ? 1 : 2;
    }

    // diagonala 1
    if (tabla[0] != ' ' && tabla[0] == tabla[4] && tabla[0] == tabla[8])
        return (tabla[0] == 'X') ? 1 : 2;

    // diagonala 2
    if (tabla[2] != ' ' && tabla[2] == tabla[4] && tabla[2] == tabla[6])
        return (tabla[2] == 'X') ? 1 : 2;

    return 0; // nimeni nu a castigat
}


int board_full() {
    for (int i = 0; i < 9; i++)
        if (tabla[i] == ' ') return 0;
    return 1;
}


int find_first_free() {
    for (int i = 0; i < 9; i++)
        if (tabla[i] == ' ')
            return i;
    return -1;
}


int client_move(int poz) {
    if (poz < 0 || poz > 8) return -1;
    if (tabla[poz] != ' ') return -2;
    tabla[poz] = 'X';
    return 1;
}
