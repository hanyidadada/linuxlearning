#include "wrap.h"
#include "epoll_loop.h"

extern struct online_s onlineuser[MAX_EVENTS];

void initlistensocket(struct myevent_s g_events[], int epfd, unsigned short port, char ipstr[]) {
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(epfd, EPOLLIN, &g_events[MAX_EVENTS]);

    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, ipstr, &(sin.sin_addr));

    Bind(lfd, &sin, sizeof(sin));

    Listen(lfd, 20);
    return ;
}

void eventset(struct myevent_s *ev, int fd, void (*call_back)(int g_efd, struct myevent_s *g_events, int fd, int events, void *arg), void *arg) {
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->status = 0;
    ev->arg = arg;
    memset(&ev->data, 0, sizeof(ev->data));
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

void acceptconn(int g_efd, struct myevent_s g_events[], int fd, int events, void *arg) {
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
        eventset(&g_events[i], cfd, servrecvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
        eventset(&g_events[MAX_EVENTS], fd, acceptconn, &g_events[MAX_EVENTS]);
        eventadd(g_efd, EPOLLIN, &g_events[MAX_EVENTS]);
    } while (0);
    printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);
    return ;
}

void servrecvdata(int g_efd, struct myevent_s g_events[], int fd, int events, void *arg) {
    struct myevent_s *ev = (struct myevent_s *)arg;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    char buffer[BUFLEN],srcname[NAMELEN], dstname[NAMELEN];;
    int len, i;
    len = recv(fd, &ev->data, sizeof(ev->data), 0);
    memset(&peeraddr, 0, sizeof(peeraddr));
    getpeername(fd, (struct sockaddr*)&peeraddr, &plen);
    if (len > 0) {
        switch (ev->data.num) {
        case 1:
            for (i = 0; i < MAX_EVENTS; i++) {
                if (onlineuser[i].status == 0) {
                    break;
                } else {
                    if (strcmp(onlineuser[i].name, ev->data.srcname) == 0) {
                        i = MAX_EVENTS + 1;
                    }  
                }
            }
            if (i >= MAX_EVENTS) {
                printf("%s: log in error[%d]\n", __func__, i);
                eventset(ev, fd, servsenddata, ev);
                ev->data.num = 6;    
                eventadd(g_efd, EPOLLOUT, ev);
                return ;
            }
            
            strcpy(onlineuser[i].name, ev->data.srcname);
            onlineuser[i].fd = ev->fd;
            onlineuser[i].status = 1;
            eventset(ev, fd, servsenddata, ev);
            ev->data.num = 5;
            eventadd(g_efd, EPOLLOUT, ev);
            printf("Clinet[%s:%u] log in as %s\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), onlineuser[i].name);
            break;
        case 2:
            for (i = 0; i < MAX_EVENTS; i++) {
                if (onlineuser[i].status == 1) {
                    if (strcmp(onlineuser[i].name, ev->data.dstname) == 0) {
                        break;
                    }        
                }
            }
            if (i == MAX_EVENTS) {
                printf("no such user %s\n", ev->data.dstname);
                eventset(ev, fd, servsenddata, ev);
                ev->data.num = 6;
                eventadd(g_efd, EPOLLOUT, ev);
                return ;
            }
            memset(buffer, 0, BUFLEN);
            memset(srcname, 0, NAMELEN);
            memset(dstname, 0, NAMELEN);
            strcpy(buffer, ev->data.buffer);
            strcpy(srcname, ev->data.srcname);
            strcpy(dstname, ev->data.dstname);
            int j;
            for (j = 0; j < MAX_EVENTS; j++) {
                if (g_events[j].fd == onlineuser[i].fd) {
                    break;
                }
            }
            eventdel(g_efd, &g_events[j]);
            eventset(&g_events[j], onlineuser[i].fd, servsenddata, &g_events[j]);
            strcpy(g_events[j].data.buffer, buffer);
            strcpy(g_events[j].data.srcname, srcname);
            strcpy(g_events[j].data.dstname, dstname);
            g_events[j].data.num = 2;
            eventadd(g_efd, EPOLLOUT, &g_events[j]);
            printf("Message form[%s:%u]:(%s send to %s) %s", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), srcname, dstname, g_events[j].data.buffer);
            eventset(ev, fd, servsenddata, ev);
            ev->data.num = 5;
            eventadd(g_efd, EPOLLOUT, ev);
            break;
        case 4:
            for (i = 0; i < MAX_EVENTS; i++) {
                if (onlineuser[i].status == 1) {
                    if (strcmp(onlineuser[i].name, ev->data.srcname) == 0) {
                        break;
                    }        
                }
            }
            if (i == MAX_EVENTS) {
                printf("Client[%s:%u] already left\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
            }
            onlineuser[i].status = 0;
            close(fd);
            break;
        default:
            break;
        }
    } else if (len == 0){
        for (i = 0; i < MAX_EVENTS; i++) {
            if (onlineuser[i].fd == fd) {
                onlineuser[i].status = 0;
            }
        }
        printf("Client[%s:%u] already left\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        close(fd);
    } else {
        close(fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return ;
}

void servsenddata(int g_efd, struct myevent_s g_events[], int fd, int events, void *arg) {
    struct myevent_s *ev = (struct myevent_s *)arg;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    int len, i;

    len = send(fd, &ev->data, sizeof(ev->data), 0);
    memset(&peeraddr, 0, sizeof(peeraddr));
    getpeername(fd, (struct sockaddr*)&peeraddr, &plen);
    if (len > 0) {
        switch (ev->data.num) {
        case 5:
        case 6:
            printf("Send to[%s:%u]:%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), ev->data.num);
            eventset(ev, fd, servrecvdata, ev);
            eventadd(g_efd, EPOLLIN, ev);
            break;
        case 2:
            printf("Server send[%s:%d] (%s send to %s): %s",inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), ev->data.srcname, ev->data.dstname, ev->data.buffer);
            eventset(ev, fd, servrecvdata, ev);
            eventadd(g_efd, EPOLLIN, ev);
        default:
            break;
        }        
    } else {
        close(ev->fd);
        eventdel(g_efd, ev);
        printf("send[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return ;
}
