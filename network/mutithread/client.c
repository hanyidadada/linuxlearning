#include "wrap.h"

int main(int argc, char *argv[]) {
    int cfd, n, len;
    short port;
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

     while (1) {
        char ch;
        int i = 0;
        memset(buffer, 0, BUFSIZ);
        printf("Enter message:");
        while ((ch = getchar())!= '\n') {
            buffer[i++] = ch;
            if (i == BUFSIZ - 2) {
                break;
            }
        }
        buffer[i++] = '\n';
        buffer[i] = '\0';
        n = write(cfd, buffer, i);
        if (strcmp(buffer, "exit\n") == 0) {
            printf("\tBye\n");
            break;
        }
        n = read(cfd, buffer, i);
        printf("Message from %s:%u: %s", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port), buffer);
    }
    close(cfd);
    exit(0);


}