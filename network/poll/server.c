#include "wrap.h"
#include <poll.h>
#include <ctype.h>

#define MAX_CONNECT 5000

int main(int argc, char *argv[]){
    int listenfd, cfd, sockfd, n, i, maxi, len, closeflag;
    int opt = 1;
    struct pollfd rfds[MAX_CONNECT];
    struct sockaddr_in serv_addr, cli_addr, peer_addr;
    socklen_t client_len, peer_len;
    char buff[BUFSIZ];
    short port;

    if (argc < 3) {
        perror("argv too few! Please run as \"./server ip port\"");
        exit(-1);
    }

    sscanf(argv[2]," %hd", &port);
    for (i = 0; i < MAX_CONNECT; i++) {
        rfds[i].fd = -1;
    }
    

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr));
    
    Bind(listenfd, &serv_addr, sizeof(serv_addr));

    Listen(listenfd, MAX_CONNECT);
    rfds[0].fd = STDIN_FILENO;
    rfds[0].events = POLLIN;
    rfds[0].revents = 0;
    rfds[1].fd = listenfd;
    rfds[1].events = POLLIN;
    rfds[1].revents = 0;
    maxi = 1;
    
    printf("Wait Connecting\n");
    while (1) {
        n = poll(rfds, maxi + 1, -1);
        if (n < 0) {
            perror("select error!");
            exit(-1);
        }
        if (rfds[1].revents == POLLIN) {
            bzero(&cli_addr, sizeof(cli_addr));
            cfd = Accept(listenfd, &cli_addr, &client_len);
            printf("Clinet %s:%u connected!\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            
            for (i = 2; i < MAX_CONNECT; i++) {
                if (rfds[i].fd < 0) {
                    rfds[i].fd = cfd;
                    rfds[i].events = POLLIN;
                    rfds[i].revents = 0;
                    break;
                }
            }
            if (i == MAX_CONNECT) {
                perror("too many connect!");
                exit(-1);
            }
            
            maxi = i > maxi ? i : maxi;
            n--;
            if (n == 0) {
                continue;
            }
        }
        
        if (rfds[0].revents == POLLIN) {
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
                for (i = 2; i < maxi + 1; i++) {
                    if (rfds[i].fd > 0) {
                        write(rfds[i].fd, buff, strlen(buff));
                        close(rfds[i].fd);
                        rfds[i].fd = -1;
                    }
                }
                close(listenfd);
                exit(0);
            } else {
                for (i = 2; i < maxi + 1; i++) {
                    if (rfds[i].fd > 0) {
                        write(rfds[i].fd, buff, strlen(buff));
                    }
                }
            }
        }

        for (i = 2; i < maxi + 1; i++) {
            if (rfds[i].fd > 0) {
                if (rfds[i].revents == POLLIN) {
                    sockfd = rfds[i].fd;
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
                        rfds[i].fd = -1;
                        close(sockfd);
                    } else {
                        printf("Message from %s:%u: %s", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), buff);
                        if (strcmp(buff, "exit\n") == 0) {
                            printf("client %s:%u Bye!\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
                            rfds[i].fd = -1;
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
}