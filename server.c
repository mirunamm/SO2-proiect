#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUF_SIZE 128

typedef struct {
    int fd1, fd2;          // socketii jucatorilor
    char name1[50], name2[50];
} GameArgs;

char tabla[9];

// --- INITIALIZARE TABLA ---
void init_tabla() {
    for (int i = 0; i < 9; i++)
        tabla[i] = ' ';
}

// --- VERIFICARE CASTIG ---
int verificare_castig() {
    int wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},
                       {0,3,6},{1,4,7},{2,5,8},
                       {0,4,8},{2,4,6}};
    for (int i = 0; i < 8; i++)
        if (tabla[wins[i][0]] != ' ' &&
            tabla[wins[i][0]] == tabla[wins[i][1]] &&
            tabla[wins[i][1]] == tabla[wins[i][2]])
            return 1; // castig
    for (int i = 0; i < 9; i++)
        if (tabla[i] == ' ')
            return 0; // joc continua
    return 2; // remiza
}

// --- TRIMITERE BOARD CATRE CLIENTI ---
void send_board(int fd1, int fd2) {
    char msg[16];
    strcpy(msg,"BOARD ");
    memcpy(msg + 6, tabla, 9);
    write(fd1, msg, 15);
    write(fd2, msg, 15);
}

// --- THREAD PENTRU JOC ---
void *thread_joc(void *arg) {
    GameArgs *g = (GameArgs*)arg;
    char buf[BUF_SIZE];
    int turn = 0; // 0 = X (fd1), 1 = O (fd2)

    init_tabla();
    write(g->fd1, "MESSAGE Joc incepe! Esti X\n", 28);
    write(g->fd2, "MESSAGE Joc incepe! Esti O\n", 28);

    while (1) {
        send_board(g->fd1, g->fd2);

        int cur_fd = (turn == 0) ? g->fd1 : g->fd2;
        write(cur_fd, "YOUR_TURN\n", 10);

        memset(buf, 0, sizeof(buf));
        ssize_t n = read(cur_fd, buf, sizeof(buf));
        if (n <= 0) break; // client deconectat

        int pos = -1;
        sscanf(buf, "MOVE %d", &pos);

        if (pos < 0 || pos > 8 || tabla[pos] != ' ') {
            // mutare invalida -> cere din nou
            continue;
        }

        tabla[pos] = (turn == 0) ? 'X' : 'O';

        int w = verificare_castig();
        if (w == 1) { // cineva a castigat
            send_board(g->fd1, g->fd2);
            if (turn == 0) {
                write(g->fd1, "WIN\n", 4);
                write(g->fd2, "LOSE\n", 5);
            } else {
                write(g->fd2, "WIN\n", 4);
                write(g->fd1, "LOSE\n", 5);
            }
            break;
        } else if (w == 2) { // remiza
            send_board(g->fd1, g->fd2);
            write(g->fd1, "DRAW\n", 5);
            write(g->fd2, "DRAW\n", 5);
            break;
        }

        turn = 1 - turn;
    }

    close(g->fd1);
    close(g->fd2);
    free(g);
    return NULL;
}

// --- MAIN SERVER ---
int main() {
    int s;
    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); exit(1); }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind"); exit(1);
    }

    if (listen(s, 10) < 0) {
        perror("listen"); exit(1);
    }

    printf("Server TicTacToe pornit pe port %d\n", PORT);

    while (1) {
        GameArgs *g = malloc(sizeof(GameArgs));

        printf("Astept jucatorul 1...\n");
        g->fd1 = accept(s, NULL, NULL);

        char buf[BUF_SIZE];
        read(g->fd1, buf, sizeof(buf));
        sscanf(buf, "NAME %49s", g->name1);

        printf("Astept jucatorul 2...\n");
        g->fd2 = accept(s, NULL, NULL);
        read(g->fd2, buf, sizeof(buf));
        sscanf(buf, "NAME %49s", g->name2);

        printf("Porneste joc intre %s si %s\n", g->name1, g->name2);

        pthread_t tid;
        pthread_create(&tid, NULL, thread_joc, g);
        pthread_detach(tid);
    }

    close(s);
    return 0;
}
