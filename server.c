#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    char p1[32];
    char p2[32];
} jucatori;

char tabla[9];

void init_tabla() {
    for (int i = 0; i < 9; i++)
        tabla[i] = ' ';
}

void afiseaza_tabla() {
    printf("\n");
    for (int i = 0; i < 9; i++) {
        if (tabla[i] == ' ')
            printf(" %d ", i);  
        else
            printf(" %c ", tabla[i]); 

        if (i % 3 != 2)
            printf("|");
        else if (i != 8)
            printf("\n---+---+---\n");
    }
    printf("\n\n");
}

int verificare_castig() {
    if (tabla[0]==tabla[1] && tabla[1]==tabla[2] && tabla[0]!=' ') return 1;
    if (tabla[3]==tabla[4] && tabla[4]==tabla[5] && tabla[3]!=' ') return 1;
    if (tabla[6]==tabla[7] && tabla[7]==tabla[8] && tabla[6]!=' ') return 1;
    if (tabla[0]==tabla[3] && tabla[3]==tabla[6] && tabla[0]!=' ') return 1;
    if (tabla[1]==tabla[4] && tabla[4]==tabla[7] && tabla[1]!=' ') return 1;
    if (tabla[2]==tabla[5] && tabla[5]==tabla[8] && tabla[2]!=' ') return 1;
    if (tabla[0]==tabla[4] && tabla[4]==tabla[8] && tabla[0]!=' ') return 1;
    if (tabla[2]==tabla[4] && tabla[4]==tabla[6] && tabla[2]!=' ') return 1;
    return 0;
}

void *thread_joc(void *arg) {
    jucatori *players = (jucatori*)arg;
    char cmd[32];
    int pos;

    printf("\n=== %s (X) vs %s (O) ===\n", players->p1, players->p2);

    while (1) {
        int rand = 0;
        init_tabla();
        afiseaza_tabla();

        while (1) {
            if (rand == 0)
                printf("%s (X), introdu comanda (MOVE n): ", players->p1);
            else
                printf("%s (O), introdu comanda (MOVE n): ", players->p2);

            fgets(cmd, sizeof(cmd), stdin);

            if (cmd[0]=='M' && cmd[1]=='O' && cmd[2]=='V' && cmd[3]=='E') {
                pos = atoi(&cmd[5]);

                if (pos < 0) {
                    printf("INVALID_MOVE (negativ)\n");
                    continue;
                }
                if (pos > 8) {
                    printf("INVALID_MOVE (prea mare)\n");
                    continue;
                }
                if (tabla[pos] != ' ') {
                    printf("INVALID_MOVE (ocupat)\n");
                    continue;
                }

                if (rand == 0) tabla[pos] = 'X';
                else tabla[pos] = 'O';

                afiseaza_tabla();

                if (verificare_castig()) {
                    if (rand == 0)
                        printf("WIN pentru %s!\n", players->p1);
                    else
                        printf("WIN pentru %s!\n", players->p2);
                    break;
                }

                rand = 1 - rand; 
            }
            else {
                printf("Comanda necunoscuta. Foloseste MOVE poz\n");
            }
        }

        char rasp[8];
        printf("Doriti rematch? (da/nu): ");
        fgets(rasp, sizeof(rasp), stdin);

        if (rasp[0]=='d' || rasp[0]=='D') {
            printf("Pornim un nou meci intre aceiasi jucatori...\n");
        } else {
            printf("Joc terminat definitiv.\n");
            break;
        }
    }

    free(players);
    return NULL;
}

int main() {
    while (1) {
        char n1[32], n2[32];

        printf("\nIntrodu numele jucatorului 1: ");
        fgets(n1, 32, stdin);
        n1[strcspn(n1, "\n")] = 0;

        printf("Introdu numele jucatorului 2: ");
        fgets(n2, 32, stdin);
        n2[strcspn(n2, "\n")] = 0;

        jucatori *players = malloc(sizeof(jucatori));
        strcpy(players->p1, n1);
        strcpy(players->p2, n2);

        pthread_t tid;
        pthread_create(&tid, NULL, thread_joc, players);
        pthread_join(tid, NULL);

        printf("\nJucatorii au terminat. Putem porni un alt joc cu alti jucatori.\n");
    }

    return 0;
}
