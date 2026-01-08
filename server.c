#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUF_SIZE 128

typedef struct {
    int fd1, fd2;          
    char name1[50], name2[50];
    char tabla[9]; 
} GameArgs;

void init_tabla(char *tabla) {
    for (int i = 0; i < 9; i++) tabla[i] = ' ';
}

int verificare_castig(char *tabla) {
    int wins[8][3] = {{0,1,2},{3,4,5},{6,7,8}, {0,3,6},{1,4,7},{2,5,8}, {0,4,8},{2,4,6}};
    for (int i = 0; i < 8; i++)
        if (tabla[wins[i][0]] != ' ' && tabla[wins[i][0]] == tabla[wins[i][1]] && tabla[wins[i][1]] == tabla[wins[i][2]])
            return 1; 
    for (int i = 0; i < 9; i++) if (tabla[i] == ' ') return 0; 
    return 2; 
}

void send_board(GameArgs *g) {
    char msg[20];
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "BOARD ");
    memcpy(msg + 6, g->tabla, 9);
    write(g->fd1, msg, 15);
    write(g->fd2, msg, 15);
}

void *thread_joc(void *arg) {
    GameArgs *g = (GameArgs*)arg;
    char buf[BUF_SIZE];
    
    while (1) { 
        init_tabla(g->tabla);
        int turn = 0; 

        write(g->fd1, "MESSAGE Incepe meci nou! Esti X\n", 32);
        write(g->fd2, "MESSAGE Incepe meci nou! Esti O\n", 32);

        while (1) { 
            send_board(g);
            int cur_fd = (turn == 0) ? g->fd1 : g->fd2;
            int opp_fd = (turn == 0) ? g->fd2 : g->fd1;

            write(cur_fd, "YOUR_TURN\n", 10);
            write(opp_fd, "MESSAGE Asteapta mutarea...\n", 28);

            memset(buf, 0, sizeof(buf));
            if (read(cur_fd, buf, sizeof(buf)) <= 0) goto cleanup;

            int pos = -1;
            if (sscanf(buf, "MOVE %d", &pos) == 1 && pos >= 0 && pos <= 8 && g->tabla[pos] == ' ') {
                g->tabla[pos] = (turn == 0) ? 'X' : 'O';
                int w = verificare_castig(g->tabla);
                if (w > 0) {
                    send_board(g);
                    if (w == 1) {
                        write(cur_fd, "WIN\n", 4);
                        write(opp_fd, "LOSE\n", 5);
                    } else {
                        write(g->fd1, "DRAW\n", 5);
                        write(g->fd2, "DRAW\n", 5);
                    }
                    sleep(3); 
                    break; 
                }
                turn = 1 - turn;
            }
        }
    }

cleanup:
    close(g->fd1); close(g->fd2); free(g);
    return NULL;
}

int main() {
    int s, opt = 1;
    struct sockaddr_in server;
    s = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));
    listen(s, 10);
    printf("Server pornit. Astept jucatori...\n");

    while (1) {
        GameArgs *g = malloc(sizeof(GameArgs));
        g->fd1 = accept(s, NULL, NULL);
        char b[BUF_SIZE]; 
        read(g->fd1, b, sizeof(b)); sscanf(b, "NAME %s", g->name1);

        g->fd2 = accept(s, NULL, NULL);
        read(g->fd2, b, sizeof(b)); sscanf(b, "NAME %s", g->name2);

        pthread_t tid;
        pthread_create(&tid, NULL, thread_joc, g);
        pthread_detach(tid);
    }
    return 0;
}
