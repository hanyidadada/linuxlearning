#include "wrap.h"

void perror_exit(char *s){
    perror(s);
    exit(-1);
}

int Socket(int domain, int type, int protocol) {
    int ret;

    if ((ret = socket(domain, type, protocol)) < 0) {
        perror_exit("create socket error:");
    }
    return ret;
}

int Bind(int fd, struct sockaddr_in *addr, socklen_t len) {
    int ret;

    if ((ret = bind(fd, (struct sockaddr*) addr, len)) < 0) {
        printf("%d\n",ret);
        perror_exit("Bind ip/port error:");
    }
    return ret;
}

int Accept(int fd, struct sockaddr_in *addr,
            socklen_t *addr_len) {
    int ret;
again:
    if ((ret = accept(fd, (struct sockaddr*) addr, addr_len)) < 0) {
        if (errno == ECONNABORTED || errno == EINTR) {
            goto again;
        } else {
            perror_exit("accept ip/port error:");
        }  
    }
    return ret;
}

int Connect(int fd, struct sockaddr_in *addr, socklen_t len) {
    int ret;
    if ((ret = connect(fd, (struct sockaddr*) addr, len)) < 0) {
        perror_exit("Connect ip/port error:");
    }
    return ret;
}

int Listen(int fd, int n) {
    int ret;
    if ((ret = listen(fd, n)) < 0) {
        perror_exit("listen error:");
    }
    return ret;
}