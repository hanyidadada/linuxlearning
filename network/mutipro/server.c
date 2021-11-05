#include "wrap.h"
#include <signal.h>
#include <ctype.h>
#include <sys/wait.h>
#define MAX_CONNECT 5

void waitchild(int signum) {
    while (waitpid(0, NULL, WNOHANG) >0);
    return ;
}


int main(int argc, char *argv[]){
    int listenfd, cfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t client_len;
    char buff[BUFSIZ];
    short port;
    pid_t pid;
    
    if (argc < 3) {
        perror("argv too few! Please run as \"./server ip port\"");
        exit(-1);
    }

    signal(SIGCHLD, waitchild);

    sscanf(argv[2]," %hd", &port);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr));
    
    Bind(listenfd, &serv_addr, sizeof(serv_addr));

    Listen(listenfd, MAX_CONNECT);

    while (1) {
        bzero(&cli_addr, sizeof(cli_addr));
        cfd = Accept(listenfd, &cli_addr, &client_len);
        pid = fork();
        if (pid < 0) {
            perror("fork error!");
            exit(-1);
        } else if (pid > 0) {
            close(cfd);
            continue;
        } else {
            int n, i;
            close(listenfd);
            printf("Clinet %s:%u connected!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            while (1) {
                memset(buff, 0, BUFSIZ);
                n = readline(cfd, buff, BUFSIZ);
                if (n < 0) {
                    perror("Read error");
                    close(cfd);
                    exit(-1);
                } else if (n == 0) {
                    continue;
                }
                
                printf("Message from %s:%u: %s", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buff);
                if (strcmp(buff, "exit\n") == 0) {
                    printf("%s:%u Bye!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                    close(cfd);
                    exit(0);
                }
                
                for (i = 0; i < n; i++) {
                    buff[i] = toupper(buff[i]);
                }
                write(cfd, buff, n);
            }
        }
    }
}