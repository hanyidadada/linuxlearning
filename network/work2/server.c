#include "wrap.h"

int main(int argc, char *argv[]){
    int sockfd, listenfd;
    struct sockaddr_in addr, clinetaddr;
    socklen_t len = sizeof(addr);
    struct in_addr ipaddr;
    short port;
    char buffer[1024];
    
    if (argc < 3){
        perror("argv too few.run as \"./a.out ip port\"");
        exit(-1);
    }
    sscanf(argv[2], " %d", &port);
    inet_pton(AF_INET, argv[1], &ipaddr);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = ipaddr;

    Bind(listenfd, &addr, len);

    Listen(listenfd, 5);

    sockfd = Accept(listenfd, &clinetaddr, &len);
    close(listenfd);
    while (1) {
        memset(buffer, 0, 1024);
        len = read(sockfd, &buffer, 1023);
        buffer[len] = '\0';
        printf("client %s:%d message: %s\n", inet_ntoa(clinetaddr.sin_addr), ntohs(clinetaddr.sin_port), buffer);
        if (strcmp(buffer, "exit") == 0) {
            printf("\tBye\n");
            break;
        }
    }
    close(sockfd);
    exit(0);
}
