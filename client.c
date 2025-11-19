#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main() {
    char nume[32];
    char cmd[32];

    printf("-CLIENT-\n\n");

    printf("Introdu numele tau: ");
    fgets(nume, sizeof(nume), stdin);
    nume[strcspn(nume, "\n")] = 0;

    printf("\nConectare la server...\n");
    printf("Trimis catre server: NUME %s\n", nume);

    printf("Asteptam un adversar...\n");
    printf("Adversar gasit: jucator2\n\n");

    printf("Rolul tau: X\n");
    printf("Adversar: jucator2 (O)\n");

    init_tabla();
    afiseaza_tabla();

    while (1) {
        printf("%s, introdu comanda (MOVE n): ", nume);
        fgets(cmd, sizeof(cmd), stdin);

        if (strncmp(cmd, "MOVE", 4) == 0) {
            int pos = atoi(&cmd[5]);

            printf("Trimis catre server: MOVE %d\n", pos);

            if (pos < 0 || pos > 8) {
                printf("Server: INVALID_MOVE\n");
                continue;
            }

            if (tabla[pos] != ' ') {
                printf("Server: INVALID_MOVE (pozitie ocupata)\n");
                continue;
            }

            tabla[pos] = 'X';
            printf("Server: MOVE_ACCEPTED\n");

            afiseaza_tabla();

            printf("Server: Adversarul muta...\n");

            int adv_pos = -1;
            for (int i = 0; i < 9; i++)
                if (tabla[i] == ' ') { adv_pos = i; break; }

            if (adv_pos == -1) {
                printf("Server: Remiza!\n");
                break;
            }

            tabla[adv_pos] = 'O';
            printf("Server: Adversar a mutat in pozitia %d\n", adv_pos);

            afiseaza_tabla();
        }
        else {
            printf("Comanda necunoscuta. Foloseste MOVE n\n");
        }
    }

    printf("\nServer: Joc terminat.\n");
    printf("Deconectare...\n");

    return 0;
}
