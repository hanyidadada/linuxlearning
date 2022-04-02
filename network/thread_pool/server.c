#include "wrap.h"
#include "epoll_loop.h"
#include "threadpool.h"
struct online_s onlineuser[MAX_EVENTS];

struct server_pack{
    int g_efd;
    int events;
    struct myevent_s *g_events;
    struct myevent_s *ev; 
};

void *server_thread(void *arg) {
    struct server_pack *args = (struct server_pack *)arg;
    if (args->events == EPOLLIN && args->ev->events == EPOLLIN) {
        args->ev->call_back(args->g_efd, args->g_events, args->ev->fd, args->ev->events, args->ev->arg);
    }
    if (args->events == EPOLLOUT && args->ev->events == EPOLLOUT) {
        args->ev->call_back(args->g_efd, args->g_events, args->ev->fd, args->ev->events, args->ev->arg);
    }
}

int main(int argc, char *argv[]) {
    unsigned short port = SERV_PORT;
    struct sockaddr_in peeraddr;
    socklen_t plen = sizeof(peeraddr);
    int g_efd;
    struct server_pack *arg;
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
    threadpool_t *pool = threadpool_create(10, 50, 100);

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

            if (duration >= 600) {
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
            arg = NULL;
            arg = (struct server_pack *)malloc(sizeof(struct server_pack));
            struct myevent_s *ev = (struct myevent_s *) events[i].data.ptr;
            arg->ev = ev;
            arg->g_efd = g_efd;
            arg->g_events = g_events;
            arg->events = events[i].events;
            eventdel(g_efd, ev);
            threadpool_add_task(pool, server_thread, (void *)arg);
        }
    }
}
