#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include "gui.h"

#define PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[512], tabla[9], msg[100], nume[50];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Conectare esuata"); return 1;
    }

    printf("Nume: "); scanf("%s", nume);
    snprintf(msg, sizeof(msg), "NAME %s\n", nume);
    write(sockfd, msg, strlen(msg));

    gui_init();

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = read(sockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        buffer[n] = '\0';

        if (strstr(buffer, "MESSAGE")) {
            char *p = strstr(buffer, "MESSAGE");
            move(1, 0); clrtoeol();
            mvprintw(1, 0, "%s", p);
            refresh();
        }

        if (strstr(buffer, "BOARD")) {
            char *p = strstr(buffer, "BOARD");
            memcpy(tabla, p + 6, 9);
            gui_draw_board(tabla);
        }

        if (strstr(buffer, "YOUR_TURN")) {
            int move_idx = gui_get_move(1);
            if (move_idx >= 0) {
                snprintf(msg, sizeof(msg), "MOVE %d\n", move_idx);
                write(sockfd, msg, strlen(msg));
            }
        }

        if (strstr(buffer, "WIN")) mvprintw(15, 0, "AI CASTIGAT! Asteapta...");
        if (strstr(buffer, "LOSE")) mvprintw(15, 0, "AI PIERDUT! Asteapta...");
        if (strstr(buffer, "DRAW")) mvprintw(15, 0, "REMIZA! Asteapta...");
        refresh();
    }

    gui_close();
    close(sockfd);
    return 0;
}
