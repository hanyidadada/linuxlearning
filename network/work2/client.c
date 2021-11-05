#include "wrap.h"

int main(int argc, char *argv[]){
    int sockfd;
    int rlen;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    struct in_addr ipaddr;
    short port;
    char buffer[1024];
    char out[124] = "Enter message: ";
    
    if (argc < 2){
        perror("argv too few.run as \"./a.out ip\"");
        exit(-1);
    }
    sscanf(argv[2], " %d", &port);
    inet_pton(AF_INET, argv[1], &ipaddr);

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = ipaddr;

    Connect(sockfd, &addr, len);

    while (1) {
        memset(buffer, 0, 1024);
        // fgets(buffer, 1024, stdin);
        write(STDOUT_FILENO, out, strlen(out));
        rlen = read(STDIN_FILENO, buffer, 1023); 
        if (buffer[rlen - 1] == '\n') {
            buffer[rlen - 1] = '\0';
            --rlen;
        } else {
            buffer[rlen] = '\0';
        }
        
        len = write(sockfd, &buffer, rlen);
        if (strcmp(buffer, "exit") == 0) {
            printf("\tBye\n");
            break;
        }
    }
    close(sockfd);
    exit(0);
}
