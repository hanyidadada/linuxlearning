#include "wrap.h"
#include <sys/select.h>

int main(int argc, char *argv[]) {
    int cfd, n, len, maxfd;
    short port;
    fd_set rset, allset;
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
    FD_ZERO(&allset);
    FD_SET(cfd, &allset);
    FD_SET(STDIN_FILENO, &allset);
    maxfd = cfd > STDIN_FILENO ? cfd + 1 : STDIN_FILENO + 1;
    while (1) {
        rset = allset;
        n = select(maxfd, &rset, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &rset)) {
            memset(buffer, 0, BUFSIZ);
            len = read(STDIN_FILENO, buffer, BUFSIZ);
            n = write(cfd, buffer, len);
            if (strcmp(buffer, "exit\n") == 0) {
                printf("\tBye\n");
                break;
            }
        }
        if (FD_ISSET(cfd, &rset)) {
            memset(buffer, 0, BUFSIZ);
            n = read(cfd, buffer, BUFSIZ);
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