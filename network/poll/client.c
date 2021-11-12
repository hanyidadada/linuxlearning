#include "wrap.h"
#include <poll.h>

int main(int argc, char *argv[]) {
    int cfd, n, len, maxfd;
    short port;
    struct pollfd rfds[2];
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
    rfds[0].fd = cfd;
    rfds[0].events = POLLIN;
    rfds[0].revents = 0;
    rfds[1].fd = STDIN_FILENO;
    rfds[1].events = POLLIN;
    rfds[1].revents = 0;
    while (1) {
        n = poll(rfds, 2, -1);
        if (rfds[1].revents == POLLIN) {
            memset(buffer, 0, BUFSIZ);
            len = read(STDIN_FILENO, buffer, BUFSIZ);
            n = write(cfd, buffer, len);
            if (strcmp(buffer, "exit\n") == 0) {
                printf("\tBye\n");
                break;
            }
        }
        if (rfds[0].revents == POLLIN) {
            memset(buffer, 0, BUFSIZ);
            n = read(cfd, buffer, BUFSIZ);
            if (n <= 0) {
                printf("server left\n");
                break;
            }
            
            printf("Message from %s:%u: %s", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port), buffer);
            if (strcmp(buffer, "exit\n") == 0) {
                printf("server let me exit\n");
                break;
            }
        }       
    }
    close(cfd);
    exit(0);
}