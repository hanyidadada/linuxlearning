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
    if (ret = listen(fd, n) < 0) {
        perror_exit("listen error:");
    }
    return ret;
}

//数据量大于以太网帧格式时候，需要封装操作
int Readn(int fd, void *buf, size_t n){
    int nleft;
    int nread;
    char *ptr;

    nleft = n;
    nread = 0;
    ptr = buf;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
            } else {
                printf("read error:%s\n", strerror(errno));
                return -1;
            }
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    
    return n - nleft;
}

int Writen(int fd, void *buf, size_t n){
    int nleft;
    int nwrite;
    char *ptr;

    nleft = n;
    nwrite = 0;
    ptr = buf;
    while (nleft > 0) {
        if ((nwrite = write(fd, ptr, nleft)) <= 0) {
            if (errno == EINTR && nwrite <0) {
                nwrite = 0;
            } else {
                printf("read error:%s\n", strerror(errno));
                return -1;
            }
        }
        nleft -= nwrite;
        ptr += nwrite;
    }
    
    return n;
}

static ssize_t my_read(int fd, char *ptr) {
    static int read_cnt;
    static char *read_ptr;
    static read_buf[100];
    if (read_cnt <= 0) {
again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR) {
                goto again;
            } else {
                return -1;
            }
        } else if (read_cnt == 0) {
            return 0;
        }
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr;
    read_ptr++;
    return 1;
}
//读取网络中的一行
int readline(int fd, void *vptr, int maxlen) {
    int i, rc;
    char c, *ptr;
    ptr = vptr;

    for (i = 1; i < maxlen; i++) {
        if ((rc = my_read(fd, &c)) == 1) {
            *ptr = c;
            ptr++;
            if (c == '\n') {
                break;
            }
        } else if (rc == 0) {
            *ptr = 0;
            return i-1;
        } else {
            return -1;
        }
    }
    *ptr = 0;
    return i;
}
