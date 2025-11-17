#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    char p1[32];
    char p2[32];
} jucatori;

char board[9];

void init_board() {
    for (int i = 0; i < 9; i++)
        board[i] = ' ';
}

void print_board() {
    printf("\n");
    printf(" %c | %c | %c\n", board[0], board[1], board[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", board[3], board[4], board[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", board[6], board[7], board[8]);
    printf("\n");
}

int check_win() {
    
    if (board[0]==board[1] && board[1]==board[2] && board[0]!=' ')
        return 1;
    if (board[3]==board[4] && board[4]==board[5] && board[3]!=' ')
        return 1;
    if (board[6]==board[7] && board[7]==board[8] && board[6]!=' ')
        return 1;


    if (board[0]==board[3] && board[3]==board[6] && board[0]!=' ')
        return 1;
    if (board[1]==board[4] && board[4]==board[7] && board[1]!=' ')
        return 1;
    if (board[2]==board[5] && board[5]==board[8] && board[2]!=' ')
        return 1;


    if (board[0]==board[4] && board[4]==board[8] && board[0]!=' ')
        return 1;
    if (board[2]==board[4] && board[4]==board[6] && board[2]!=' ')
        return 1;

    return 0;
}

void *game_thread(void *arg) {
    jucatori *players = (jucatori *)arg;
    int turn = 0;
    char cmd[32];
    int pos;

    printf("\n=== Joc inceput intre %s (X) si %s (O) ===\n",
           players->p1, players->p2);

    init_board();
    print_board();

    while (1) {

        if (turn == 0)
            printf("%s (X), introdu comanda (MOVE x): ", players->p1);
        if (turn == 1)
            printf("%s (O), introdu comanda (MOVE x): ", players->p2);

        fgets(cmd, sizeof(cmd), stdin);

        if (cmd[0]=='M' && cmd[1]=='O' && cmd[2]=='V' && cmd[3]=='E') {

            pos = atoi(&cmd[5]);

 
            if (pos < 0) {
                printf("SERVER: INVALID_MOVE (negativ)\n");
                continue;
            }
            if (pos > 8) {
                printf("SERVER: INVALID_MOVE (prea mare)\n");
                continue;
            }
            if (board[pos] != ' ') {
                printf("SERVER: INVALID_MOVE (ocupat)\n");
                continue;
            }


            if (turn == 0) board[pos] = 'X';
            else board[pos] = 'O';

            printf("Tabla ");
            for (int i = 0; i < 9; i++)
                printf("%c", board[i]);
            printf("\n");

            print_board();

            if (check_win()) {
                if (turn == 0)
                    printf("CASTIGATOR: %s\n", players->p1);
                else
                    printf("CASTIGATOR: %s\n", players->p2);
                break;
            }

            if (turn == 0) turn = 1;
            else turn = 0;
        }
        else {
            printf("Comanda necunoscuta (foloseste MOVE poz)\n");
        }
    }

    printf("=== Meci terminat ===\n");
    free(players);
    return NULL;
}

int main() {
    while (1) {
        char n1[32], n2[32];

        printf("\nIntrodu numele jucatorului 1: ");
        fgets(n1, 32, stdin); n1[strcspn(n1, "\n")] = 0;

        printf("Introdu numele jucatorului 2: ");
        fgets(n2, 32, stdin); n2[strcspn(n2, "\n")] = 0;

        jucatori *players = malloc(sizeof(jucatori));
        strcpy(players->p1, n1);
        strcpy(players->p2, n2);

        pthread_t tid;
        pthread_create(&tid, NULL, game_thread, players);

        pthread_join(tid, NULL);

        printf("Meciul s-a încheiat, pornim altul...\n");
    }

    return 0;
}
