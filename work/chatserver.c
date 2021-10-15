#include "chat.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

OnlineUser *allOnlineUsr = NULL;

int addUser(OnlineUser *temp);
int delUser(char *name);
int searchUsr(char *name);

int main(void){
    int pfd, ret, bufferlen, privfd;
    Pack  dataPack;
    OnlineUser *temp = NULL;
    char privFifoName[128];

    unlink(PUBLIC_FIFO); // 删除之前的同名管道文件
    ret = mkfifo(PUBLIC_FIFO, 0777); // 创建FIFO
    if (ret < 0) {
        perror("mkfifo error!");
        exit(1);
    }
    pfd = open(PUBLIC_FIFO, O_RDONLY);
    if (pfd < 0) {
        perror("open fifo error!");
        exit(2);
    }
    
    do{
        memset(&dataPack, 0, sizeof(dataPack));
        dataPack.num = -1;
        bufferlen = read(pfd, &dataPack, sizeof(Pack));
        if (bufferlen < 0) {
            perror("read FIFO error!");
        }
        switch (dataPack.num)
        {
            case 0:
                temp = (OnlineUser*)malloc(sizeof(OnlineUser));
                strcpy(temp->name, dataPack.src.name);
                sprintf(privFifoName,"%sand%d", dataPack.src.name, dataPack.src.pid);
                temp->privfd = open(privFifoName, O_WRONLY);
                ret = addUser(temp);
                if (ret != 0) {
                    perror("add User error:");
                    exit(-1);
                }
                printf("SERVER:User:%s, pid:%d log in!\n", dataPack.src.name, dataPack.src.pid);
                temp = NULL;
                break;
            case 1:
                privfd = searchUsr(dataPack.dest.name);
                if (privfd < 0) {
                    privfd = searchUsr(dataPack.src.name);
                    dataPack.num = 2;
                    strcpy(dataPack.src.name, dataPack.dest.name);
                    ret = write(privfd, &dataPack, sizeof(dataPack));
                    printf("User %s is offline\n", dataPack.src.name);
                    if (ret < 0) {
                        perror("write User error:");
                        exit(-1);
                    }
                } else {
                    ret = write(privfd, &dataPack, sizeof(dataPack));
                    printf("SERVER:src usr(%s) dest usr(%s) data:%s", dataPack.src.name, dataPack.dest.name, dataPack.data);
                    if (ret < 0) {
                        perror("writ User error:");
                        exit(-1);
                    }
                }
                break;
            case 3:
                privfd = searchUsr(dataPack.src.name);
                if (privfd < 0) {
                    perror("can't find the user:");
                } else {
                    ret = delUser(dataPack.src.name);
                    close(privfd);
                    if (ret < 0) {
                        perror("usr quit error!");
                    }
                    printf("SERVER:User:%s, pid:%d log out!\n", dataPack.src.name, dataPack.src.pid);
                }             
                break;
            default:
                break;
        }
    }while(1);
    close(pfd);
    unlink(PUBLIC_FIFO);
    return 0;
}

int addUser(OnlineUser *temp)
{
    OnlineUser *p = allOnlineUsr;
    if (p == NULL) {
        allOnlineUsr = temp;
    } else {
        while (p->next != NULL){
            p = p->next;
        }
        p->next = temp;       
    }
    return 0;
}

int searchUsr(char *name)
{
    OnlineUser *p = allOnlineUsr;
    while (p) {
        if(strcmp(name, p->name) == 0) {
            return p->privfd;
        }
        p = p->next;
    }
    return -1;  
}

int delUser(char *name)
{
    OnlineUser *p = allOnlineUsr;
    OnlineUser *q = p->next;
    if (strcmp(name, p->name) == 0) {
        allOnlineUsr = q;
        p->next = NULL;
        free(p);
        return 0;
    }
    while (q) {
        if(strcmp(name, q->name) == 0) {
            p->next = q->next;
            q->next = NULL;
            free(q);
            return 0;
        }
        p = p->next;
        q = p->next;
    }
    return -1;
}
