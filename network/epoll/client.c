#include "wrap.h"
#include <sys/epoll.h>

int main(int argc, char *argv[]) {
    int cfd, n, i,len, epfd;
    short port;
    struct epoll_event tmp, evts[2];
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZ];

    if (argc < 3) {
        perror("argv too few! Please run as \"./client ip port\"");
        exit(-1);
    }
    
    sscanf(argv[2], " %hd", &port);

    cfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr.s_addr));

    Connect(cfd, &serv_addr, sizeof(serv_addr));
    epfd = epoll_create(2);
    if (epfd < 0) {
        perror("epoll create err\n");
        exit(-1);
    }
    tmp.events = EPOLLIN;
    tmp.data.fd = cfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tmp);
    tmp.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &tmp);

    while (1) {
        n = epoll_wait(epfd, evts, 2, -1);
        for (i = 0; i < n; i++) {
            if (evts[i].events != EPOLLIN) {
                continue;
            }
            if (evts[i].data.fd == STDIN_FILENO) {
                memset(buffer, 0, BUFSIZ);
                len = read(STDIN_FILENO, buffer, BUFSIZ);
                len = write(cfd, buffer, len);
                if (strcmp(buffer, "exit\n") == 0) {
                    printf("\tBye\n");
                    close(cfd);
                    exit(0);
                }
            } else if (evts[i].data.fd == cfd) {
                memset(buffer, 0, BUFSIZ);
                len = read(cfd, buffer, BUFSIZ);
                if (len == 0) {
                    printf("server already leave\n");
                    close(cfd);
                    exit(0);
                }
                printf("Message from %s:%u: %s", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port), buffer);
            }
        }           
    }
    close(cfd);
    exit(0);
}