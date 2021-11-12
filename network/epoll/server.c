#include "wrap.h"
#include <sys/epoll.h>
#include <ctype.h>

#define MAX_CONNECT 5000

int main(int argc, char *argv[]){
    int listenfd, cfd, epfd, sockfd, n, i, j,len, closeflag;
    int opt = 1;
    struct epoll_event  tmp, evts[MAX_CONNECT];
    struct sockaddr_in serv_addr, cli_addr, peer_addr;
    socklen_t client_len, peer_len;
    char buff[BUFSIZ];
    short port;

    if (argc < 3) {
        perror("argv too few! Please run as \"./server ip port\"");
        exit(-1);
    }

    sscanf(argv[2]," %hd", &port);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr));
    
    Bind(listenfd, &serv_addr, sizeof(serv_addr));

    Listen(listenfd, MAX_CONNECT);
    epfd = epoll_create(MAX_CONNECT);
    if (epfd < 0) {
        perror("epfd create error");
        exit(-1);
    }
    
    tmp.data.fd = STDIN_FILENO;
    tmp.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &tmp);
    tmp.events = EPOLLIN;
    tmp.data.fd = listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &tmp);
    
    printf("Wait Connecting\n");
    while (1) {
        n = epoll_wait(epfd, evts, MAX_CONNECT, -1);
        if (n < 0) {
            perror("epoll error!");
            exit(-1);
        }
        for (i = 0; i < n; i++) {
            if (evts[i].events != EPOLLIN) {
                continue;
            }
            if (evts[i].data.fd == listenfd) {
                bzero(&cli_addr, sizeof(cli_addr));
                cfd = Accept(listenfd, &cli_addr, &client_len);
                printf("Clinet %s:%u connected!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                
                tmp.data.fd = cfd;
                tmp.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tmp);
            } else if (evts[i].data.fd == STDIN_FILENO) {
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
                    close(listenfd);
                    exit(0);
                }
            } else {
                sockfd = evts[i].data.fd;
                memset(&peer_addr, 0, sizeof(peer_addr));
                peer_len = sizeof(peer_addr);
                getpeername(sockfd, (struct sockaddr*)&peer_addr, &peer_len);
                memset(buff, 0, BUFSIZ);
                len = read(sockfd, buff, BUFSIZ);
                if (len < 0) {
                    perror("Read error");
                    closeflag = 1;
                } else if (len == 0) {
                    printf("client  %s:%u already leave!\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
                    closeflag = 1;
                }
                if (closeflag) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                    close(sockfd);
                } else {
                    printf("Message from %s:%u: %s", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), buff);
                    if (strcmp(buff, "exit\n") == 0) {
                        printf("client %s:%u Bye!\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
                        epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                        close(sockfd);
                        continue;
                    }
                    for (j = 0; j < len; j++) {
                        buff[j] = toupper(buff[j]);
                    }
                    write(sockfd, buff, len);
                }  
            }                
        }
    } 
}