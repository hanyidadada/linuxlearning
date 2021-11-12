#ifndef __WRAP_H
#define __WRAP_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX_EVENTS 1024
#define NAMELEN    12
#define BUFLEN     4096
#define SERV_PORT  8090
struct data_s{
    int num;            // 1.以buffer中的name登录 2.发送数据给目的用户dstname 3. dstname目的用户不在线 4.退出 5.服务器发给用户表示操作成功 6操作失败
    char srcname[NAMELEN];
    char dstname[NAMELEN]; //目的用户名
    char buffer[BUFLEN]; // 数据
};

struct myevent_s{
    int fd; // 监听文件描述符
    int events; // 监听事件
    void *arg; // 参数
    void (*call_back)(int g_efd, struct myevent_s *g_events, int fd, int events, void *arg); // 回调函数
    int status; // 是否监听标志位
    struct data_s data; // 缓存
    long last_active; // 添加或修改到红黑树时间
};

int Socket(int domain, int type, int protocol);
int Bind(int fd, struct sockaddr_in *addr, socklen_t len);
int Accept(int fd, struct sockaddr_in *addr, socklen_t *addr_len);
int Connect(int fd, struct sockaddr_in *addr, socklen_t len);
int Listen(int fd, int n);

#endif