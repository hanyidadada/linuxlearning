#include "wrap.h"
#include "epoll_loop.h"
#define CLINETOPNUM 3

int logflag;

int main(int argc, char *argv[]) {
    unsigned short port = SERV_PORT;
    struct sockaddr_in peeraddr;
    char srcname[NAMELEN], dstname[NAMELEN];
    struct epoll_event tmp, evts[2];
    socklen_t plen = sizeof(peeraddr);
    int g_efd, len, epfd, n, i;
    char buffer[BUFSIZ];
    struct data_s sdata, rdata;
    
    char ipstr[13] = "127.0.0.1";

    if (argc == 2) {
        port = atoi(argv[1]);
    }
    g_efd = epoll_create(CLINETOPNUM + 1);
    if (g_efd < 0) {
        printf("Create efd in %s err %s\n", __func__, strerror(errno));
        exit(-1);
    }
    int cfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, ipstr, &(sin.sin_addr));

    Connect(cfd, &sin, sizeof(sin));

    printf("client running:ip[%s]port[%d]\n", ipstr, port);

    printf("Enter your name:");
    fflush(stdout);
    len = read(STDIN_FILENO, srcname, NAMELEN);
    if (len < 0) {
        printf("read error!\n");
        close(cfd);
        exit(-1);
    }
    if (srcname[len -1] == '\n') {
        srcname[len -1] = '\0'; 
    }
    memset(&sdata, 0, sizeof(sdata));
    strcpy(sdata.srcname, srcname);
    sdata.num = 1;
    len = send(cfd, &sdata, sizeof(sdata), 0);
    if (len < 0) {
        printf("send error!\n");
        close(cfd);
        exit(-1);
    }
    memset(&rdata, 0, sizeof(rdata));
    len = recv(cfd, &rdata, sizeof(rdata), 0);
    if (len < 0) {
        printf("recv error!\n");
        close(cfd);
        exit(-1);
    }
    if (rdata.num == 6) {
        perror("log in failed");
        close(cfd);
        exit(-1);
    } else if (rdata.num == 5) {
        printf("log in success\n");
    }
    
   
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
    int opnum;
    while (1) {
        n = epoll_wait(epfd, evts, 2, -1);
        for (i = 0; i < n; i++) {
            if (evts[i].events != EPOLLIN) {
                continue;
            }
            if (evts[i].data.fd == STDIN_FILENO) {
                read(STDIN_FILENO, buffer, BUFSIZ);
                sscanf(buffer, "%d ", &opnum);
                switch (opnum) {
                case 1:
                    memset(dstname, 0, NAMELEN);
                    memset(sdata.buffer, 0, sizeof(sdata.buffer));
                    int j = 2;
                    while (buffer[j] != ' ') {
                        dstname[j-2] = buffer[j];
                        j++;
                    }
                    
                    strcpy(sdata.buffer, buffer+j+1);                    
                    sdata.num = 2;
                    strcpy(sdata.dstname, dstname);
                    send(cfd, &sdata, sizeof(sdata), 0);
              retry: len = recv(cfd, &rdata, sizeof(rdata), 0);
                    if (len < 0) {
                        perror("epoll create err\n");
                        exit(-1);
                    }
                    if (rdata.num == 6) {
                        printf("dstuser is not online\n");
                        break;
                    } else if (rdata.num == 2) {
                        printf("Message from %s: %s", rdata.srcname, rdata.buffer);
                        goto retry;
                    } else if (rdata.num == 5) {
                        printf("send success\n");
                    }
                    break;
                case 2:
                    sdata.num = 4;
                    send(cfd, &sdata, sizeof(sdata), 0);
                    close(cfd);
                    exit(0);
                default:
                    break;
                }
            } else if (evts[i].data.fd == cfd) {
                memset(&rdata, 0, sizeof(rdata));
                len = read(cfd, &rdata, sizeof(rdata));
                if (len == 0) {
                    printf("server already leave\n");
                    close(cfd);
                    exit(0);
                }
                if (rdata.num == 2) {
                    printf("Message from %s: %s", rdata.srcname, rdata.buffer);
                }   
            }
        }           
    }
    close(cfd);
    exit(0);
}
