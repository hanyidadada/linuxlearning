#include "wrap.h"
#include <sys/select.h>
#include <ctype.h>

#define MAX_CONNECT 5

int main(int argc, char *argv[]){
    int listenfd, cfd, sockfd, maxfd,n, i, maxi, len, closeflag;
    int clientfd[FD_SETSIZE];
    int opt = 1;
    fd_set rset, allset;
    struct sockaddr_in serv_addr, cli_addr, peer_addr;
    socklen_t client_len, peer_len;
    char buff[BUFSIZ];
    short port;

    if (argc < 3) {
        perror("argv too few! Please run as \"./server ip port\"");
        exit(-1);
    }

    sscanf(argv[2]," %hd", &port);
    for (i = 0; i < FD_SETSIZE; i++) {
        clientfd[i] = -1;
    }
    maxi = 0;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr));
    
    Bind(listenfd, &serv_addr, sizeof(serv_addr));

    Listen(listenfd, MAX_CONNECT);
    
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    FD_SET(STDIN_FILENO, &allset);
    maxfd = listenfd + 1;
    printf("Wait Connecting\n");
    while (1) {
        rset = allset;
        n = select(maxfd, &rset, NULL, NULL, NULL);
        if (n < 0) {
            perror("select error!");
            exit(-1);
        }
        if (FD_ISSET(listenfd, &rset)) {
            bzero(&cli_addr, sizeof(cli_addr));
            cfd = Accept(listenfd, &cli_addr, &client_len);
            printf("Clinet %s:%u connected!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            maxfd = cfd + 1 > maxfd ? cfd + 1 : maxfd;
            FD_SET(cfd, &allset);
            for (i = 0; i < FD_SETSIZE; i++) {
                if (clientfd[i] < 0) {
                    clientfd[i] = cfd;
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                perror("too many connect!");
                exit(-1);
            }
            
            maxi = i > maxi ? i : maxi;
            n--;
            if (n == 0) {
                continue;
            }
        }
        
        if (FD_ISSET(STDIN_FILENO, &rset)) {
            memset(buff, 0, BUFSIZ);
AGAIN:      len = read(STDIN_FILENO, buff, BUFSIZ);
            if (len < 0) {
                if (errno == EINTR) {
                    goto AGAIN;
                }
                perror("read from stdin error");
                exit(0);
            }
            if (strcmp(buff, "exit\n") == 0) {
                printf("server begain to leave\n");
                for (i = 0; i < maxi + 1; i++) {
                    if (clientfd[i] > 0) {
                        write(clientfd[i], buff, strlen(buff));
                        close(clientfd[i]);
                    }
                }
                close(listenfd);
                exit(0);
            }
        }

        for (i = 0; i < maxi + 1; i++) {
            if (FD_ISSET(clientfd[i], &rset)) {
                sockfd = clientfd[i];
                memset(&peer_addr, 0, sizeof(peer_addr));
                peer_len = sizeof(peer_addr);
                getpeername(sockfd, (struct sockaddr*)&peer_addr, &peer_len);
                memset(buff, 0, BUFSIZ);
                len = read(sockfd, buff, BUFSIZ);
                if (len < 0) {
                    perror("Read error");
                    closeflag = 1;
                } else if (len == 0) {
                    printf("client already leave!\n");
                    closeflag = 1;
                }
                if (closeflag) {
                    clientfd[i] = -1;
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                } else {
                    printf("Message from %s:%u: %s", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), buff);
                    if (strcmp(buff, "exit\n") == 0) {
                        printf("client %s:%u Bye!\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
                        clientfd[i] = -1;
                        FD_CLR(sockfd, &allset);
                        close(sockfd);
                        n--;
                        if (n == 0) {
                            break;
                        }
                        continue;
                    }
                    int j;
                    for (j = 0; j < len; j++) {
                        buff[j] = toupper(buff[j]);
                    }
                    write(sockfd, buff, len);
                }
                n--;
                if (n == 0) {
                    break;
                }   
            }
        }
    }
}