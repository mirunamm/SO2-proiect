#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "gui.h"

#define PORT 12345
#define BUF_SIZE 256

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[BUF_SIZE];
    char tabla[9];
    int your_turn = 0;

    // === CREARE SOCKET ===
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); exit(1); }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect"); exit(1);
    }

    // === CITIRE NUME JUCATOR ===
    char name[50];
    printf("Introdu numele jucatorului: ");
    fflush(stdout);
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0'; // elimin newline

    char msg[64];
    snprintf(msg, sizeof(msg), "NAME %s\n", name);
    write(sock, msg, strlen(msg));

    // === INIT GUI ===
    gui_init();

    // === LOOP PRINCIPAL ===
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        ssize_t n = read(sock, buffer, BUF_SIZE - 1);
        if (n <= 0) {
            mvprintw(20, 0, "Serverul a inchis conexiunea");
            refresh();
            break;
        }
        buffer[n] = '\0';

        if (strncmp(buffer, "BOARD", 5) == 0) {
            memcpy(tabla, buffer + 6, 9);
            gui_draw_board(tabla);
        }
        else if (strncmp(buffer, "YOUR_TURN", 9) == 0) {
            your_turn = 1;
            int move = gui_get_move(your_turn);
            if (move >= 0) {
                snprintf(msg, sizeof(msg), "MOVE %d\n", move);
                write(sock, msg, strlen(msg));
            }
            your_turn = 0;
        }
        else if (strncmp(buffer, "WAIT", 4) == 0) {
            your_turn = 0;
        }
        else if (strncmp(buffer, "WIN", 3) == 0 ||
                 strncmp(buffer, "LOSE", 4) == 0 ||
                 strncmp(buffer, "DRAW", 4) == 0) {
            mvprintw(15, 0, "%s", buffer);
            refresh();
            getch();
            break;
        }
    }

    gui_close();
    close(sock);
    return 0;
}
