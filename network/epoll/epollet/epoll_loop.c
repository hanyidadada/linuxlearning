#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_EVENTS 1024
#define BUFLEN     4096
#define SERV_PORT  8090

struct myevent_s{
    int fd; // 监听文件描述符
    int events; // 监听事件
    void *arg; // 参数
    void (*call_back)(int fd, int events, void *arg); // 回调函数
    int status; // 是否监听标志位
    char buf[BUFLEN]; // 缓存
    int len; // 数据长度
    long last_active; // 添加或修改到红黑树时间
};

int g_efd;
struct myevent_s g_events[MAX_EVENTS + 1] ;
void initlistensocket(int epfd, unsigned short port, char ipstr[]);
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int fd, int events, void *arg), void *arg);
void eventadd(int epfd, int events, struct myevent_s *ev);
void eventdel(int epfd, struct myevent_s *ev);
void acceptconn(int fd, int events, void *arg);
void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

int main(int argc, char *argv[]) {
    unsigned short port = SERV_PORT;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    char ipstr[13];
    if (argc < 2){
        perror("too few argv. run as \"./a.out ip <port>\"");
        exit(-1);
    }
    memset(ipstr, 0, 13);
    strcpy(ipstr, argv[1]);
    if (argc == 3) {
        port = atoi(argv[2]);
    }
    g_efd = epoll_create(MAX_EVENTS + 1);
    if (g_efd < 0) {
        printf("Create efd in %s err %s\n", __func__, strerror(errno));
        exit(-1);
    }

    initlistensocket(g_efd, port, ipstr);

    struct epoll_event events[MAX_EVENTS + 1];
    printf("server running:ip[%s]port[%d]\n", ipstr, port);
    int checkpos = 0, i;
    while (1) {
        long now = time(NULL);
        for (i = 0; i < 100; i++, checkpos++) {
            if (checkpos == MAX_EVENTS) {
                checkpos = 0;
            }
            if (g_events[checkpos].status != 1) {
                continue;
            }
            long duration = now - g_events[checkpos].last_active;

            if (duration >= 60) {
                memset(&peeraddr, 0, sizeof(peeraddr));
                plen = sizeof(peeraddr);
                getpeername(g_events[checkpos].fd, (struct sockaddr *)&peeraddr, &plen);
                printf("client[%s:%u] timeout\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
                close(g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);
            }  
        }
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nfd < 0){
            printf("epoll_wait error, exit\n");
            break;
        }
        for (i = 0; i < nfd; i++) {
            struct myevent_s *ev = (struct myevent_s *) events[i].data.ptr;
            if ((events[i].events == EPOLLIN) && (ev->events == EPOLLIN)) {
                ev->call_back(ev->fd, ev->events, ev->arg);
            }
            if ((events[i].events == EPOLLOUT) && (ev->events == EPOLLOUT)) {
                ev->call_back(ev->fd, ev->events, ev->arg);
            }
        }
    }
    
}

void initlistensocket(int epfd, unsigned short port, char ipstr[]) {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(epfd, EPOLLIN, &g_events[MAX_EVENTS]);

    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, ipstr, &(sin.sin_addr));

    bind(lfd, (struct sockaddr*)&sin, sizeof(sin));

    listen(lfd, 20);

    return ;
}

void eventset(struct myevent_s *ev, int fd, void (*call_back)(int fd, int events, void *arg), void *arg) {
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->status = 0;
    ev->arg = arg;
    memset(ev->buf, 0, BUFLEN);
    ev->len = 0;
    ev->last_active = time(NULL);

    return ;
}

void eventadd(int epfd, int events, struct myevent_s *ev) {
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if (ev->status == 1) {
        op = EPOLL_CTL_MOD;
    } else {
        ev->status = 1;
        op = EPOLL_CTL_ADD;
    }
    if (epoll_ctl(epfd, op, ev->fd, &epv) < 0) {
        printf("event add failed(fd: %d), events[%d]\n", ev->fd, events);
    } else {
        printf("event add success(fd: %d), events[%d]\n", ev->fd, events);
    }
    return ;
}

void eventdel(int epfd, struct myevent_s *ev) {
    struct epoll_event epv = {0, {0}};
    if (ev->status != 1) {
        return ;
    }
    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &epv);

    return ;
}

void acceptconn(int fd, int events, void *arg) {
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd, i;
    if ((cfd = accept(fd, (struct sockaddr*)&cin, &len)) == -1) {
        if (errno != EAGAIN && errno != EINTR){
            perror("accept error!");
            return ;
        }
        printf("%s :accept %s\n", __func__, strerror(errno));
        return ;
    }

    do {
        for (i = 0; i < MAX_EVENTS; i++) {
            if (g_events[i].status == 0) {
                break;
            }
        }
        if (i == MAX_EVENTS) {
            printf("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
        }
        
        int flag = 0;
        if ((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0) {
            printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
            break ;
        }
        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
    } while (0);
    printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);
    return ;
}

void recvdata(int fd, int events, void *arg) {
    struct myevent_s *ev = (struct myevent_s *)arg;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    char buffer[BUFLEN];
    int len;

    len = recv(fd, ev->buf, sizeof(ev->buf), 0);
    eventdel(g_efd, ev);
    memset(&peeraddr, 0, sizeof(peeraddr));
    getpeername(fd, (struct sockaddr*)&peeraddr, &plen);
    if (len > 0) {
        ev->len = len;
        ev->buf[len] = '\0';
        printf("Message form[%s:%u]:%s", inet_ntoa(peeraddr.sin_addr), 
                ntohs(peeraddr.sin_port), ev->buf);
        memset(buffer, 0, BUFLEN);
        strcpy(buffer, ev->buf);
        eventset(ev, fd, senddata, ev);
        strcpy(ev->buf, buffer);
        ev->len = len;
        eventadd(g_efd, EPOLLOUT, ev);
    } else if (len == 0){
        printf("Clinet[%s:%u] already left\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        close(fd);
    } else {
        close(fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return ;
}

void senddata(int fd, int events, void *arg) {
    struct myevent_s *ev = (struct myevent_s *)arg;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    int len, i;
    for (i = 0; i < ev->len; i++) {
            ev->buf[i] = toupper(ev->buf[i]);
        }
    len = send(fd, ev->buf, ev->len, 0);
    memset(&peeraddr, 0, sizeof(peeraddr));
    getpeername(fd, (struct sockaddr*)&peeraddr, &plen);
    if (len > 0) {
        printf("Send to[%s:%u]:%s", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), ev->buf);
        eventdel(g_efd, ev);
        eventset(ev, fd, recvdata, ev);
        eventadd(g_efd, EPOLLIN, ev);
    } else {
        close(ev->fd);
        eventdel(g_efd, ev);
        printf("send[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return ;
}
