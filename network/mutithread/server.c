#include "wrap.h"
#include <ctype.h>
#include <pthread.h>
//netstat -tunlp 
#define MAX_CONNECT 5

struct s_info{
    int cfd;
    struct sockaddr_in cli_addr;
};

void *server_thread(void *arg) {
    int n, i;
    char buff[BUFSIZ];
    struct s_info *info = (struct s_info *)arg;

    printf("Clinet %s:%u connected!\n", inet_ntoa(info->cli_addr.sin_addr), ntohs(info->cli_addr.sin_port));
    while (1) {
        memset(buff, 0, BUFSIZ);
        n = readline(info->cfd, buff, BUFSIZ);
        if (n < 0) {
            perror("Read error");
            close(info->cfd);
            pthread_exit(NULL);
        } else if (n == 0) {
            continue;
        }
        
        printf("Message from %s:%u: %s", inet_ntoa(info->cli_addr.sin_addr), ntohs(info->cli_addr.sin_port), buff);
        if (strcmp(buff, "exit\n") == 0) {
            printf("%s:%u Bye!\n", inet_ntoa(info->cli_addr.sin_addr), ntohs(info->cli_addr.sin_port));
            close(info->cfd);
            pthread_exit(NULL);;
        }
        
        for (i = 0; i < n; i++) {
            buff[i] = toupper(buff[i]);
        }
        write(info->cfd, buff, n);
    }
}


int main(int argc, char *argv[]){
    int listenfd, cfd;
    int i = 0;
    int opt = 1;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t client_len;
    pthread_t tid;
    struct s_info tinfo[MAX_CONNECT];
    short port;
    
    if (argc < 3) {
        perror("argv too few! Please run as \"./server ip port\"");
        exit(-1);
    }

    sscanf(argv[2]," %hd", &port);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //设置端口复用

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr));
    
    Bind(listenfd, &serv_addr, sizeof(serv_addr));

    Listen(listenfd, MAX_CONNECT);

    while (1) {
        bzero(&cli_addr, sizeof(cli_addr));
        bzero(&tinfo[i], sizeof(struct s_info));
        cfd = Accept(listenfd, &cli_addr, &client_len);
        tinfo[i].cfd = cfd;
        tinfo[i].cli_addr = cli_addr;
        pthread_create(&tid, NULL, server_thread, (void *)&(tinfo[i]));
        pthread_detach(tid);
        i = (i + 1) % MAX_CONNECT;
    }
}