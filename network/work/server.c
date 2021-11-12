#include "wrap.h"
#include "epoll_loop.h"
struct online_s onlineuser[MAX_EVENTS];

int main(int argc, char *argv[]) {
    unsigned short port = SERV_PORT;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    int g_efd;
    struct myevent_s g_events[MAX_EVENTS + 1] ;
   
    char ipstr[13] = "127.0.0.1";
    if (argc == 2) {
        port = atoi(argv[1]);
    }
    g_efd = epoll_create(MAX_EVENTS + 1);
    if (g_efd < 0) {
        printf("Create efd in %s err %s\n", __func__, strerror(errno));
        exit(-1);
    }

    initlistensocket(g_events, g_efd, port, ipstr);

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
                int j;
                for (j = 0; j < MAX_EVENTS; j++) {
                    if (onlineuser[j].fd == g_events[checkpos].fd) {
                        if (onlineuser[j].status == 1) {
                            onlineuser[j].status = 0;
                        }   
                    }
                }
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
                ev->call_back(g_efd, g_events, ev->fd, ev->events, ev->arg);
            }
            if ((events[i].events == EPOLLOUT) && (ev->events == EPOLLOUT)) {
                ev->call_back(g_efd, g_events, ev->fd, ev->events, ev->arg);
            }
        }
    }
}
