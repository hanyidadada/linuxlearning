#ifndef __EPOLL_LOOP_H
#define __EPOLL_LOOP_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "wrap.h"

#define MAX_EVENTS 1024
#define NAMELEN    12
#define BUFLEN     4096
#define SERV_PORT  8090

struct online_s{
    int status;
    char name[NAMELEN];
    int fd;
};

void initlistensocket(struct myevent_s g_events[], int epfd, unsigned short port, char ipstr[]);
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int g_efd, struct myevent_s *g_events, int fd, int events, void *arg), void *arg);
void eventadd(int epfd, int events, struct myevent_s *ev);
void eventdel(int epfd, struct myevent_s *ev);
void acceptconn(int g_efd, struct myevent_s g_events[], int fd, int events, void *arg);
void servrecvdata(int g_efd, struct myevent_s g_events[], int fd, int events, void *arg);
void servsenddata(int g_efd, struct myevent_s g_events[], int fd, int events, void *arg);
#endif