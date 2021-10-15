#ifndef __CHAT_H
#define __CHAT_H
#include <unistd.h>
#define MAX_DATA_LEN 256
#define USRNAME_LEN  20
#define PUBLIC_FIFO  "PublicFifoName"

typedef struct tagOnlineUser
{
    char name[USRNAME_LEN];
    int privfd;
    struct tagOnlineUser *next;
}OnlineUser;

typedef struct tagUsr {
    char name[USRNAME_LEN];
    pid_t pid;
}User;


typedef struct tagPack
{
    int num;
    User src;
    User dest;
    char data[MAX_DATA_LEN];
}Pack;
/* 数据包格式：
    协议号|源用户名|目的用户名|数据包
    0|用户名    登录
    1|用户名|用户名|数据  聊天包
    2|用户名    不在线
    3|用户名    退出           */
#endif