#include "chat.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int pfd, privfd, keyfd, ret, attr;
    User myInfo;
    Pack sendPack,recvPack;
    char privFifoName[128];
    char buffer[MAX_DATA_LEN + USRNAME_LEN + 1];
    if (argc < 2){
        perror("arg num error!");
        return -1;
    }

    ret = access(PUBLIC_FIFO, F_OK);
    if (ret < 0) {
        perror("public FIFO is not ok!");
        return -1;
    }
    
    pfd = open(PUBLIC_FIFO, O_WRONLY);
    if (pfd < 0) {
        perror("open public FIFO error!");
        return -1;
    }
    strcpy(myInfo.name, argv[1]);
    myInfo.pid = getpid();
    sprintf(privFifoName,"%sand%d", myInfo.name, myInfo.pid);
    unlink(privFifoName);
    ret = mkfifo(privFifoName, 0777);
    privfd = open(privFifoName, O_RDONLY|O_NONBLOCK);
    if (pfd < 0) {
        perror("open private FIFO error!");
        return -1;
    }
    keyfd = open("/dev/tty", O_RDONLY);
    //获取当前键盘的设置属性
    attr = fcntl(keyfd, F_GETFL);
    //设置键盘非阻塞方式读取
    fcntl(keyfd, F_SETFL, attr|O_NONBLOCK);

    
    memset(&sendPack, 0, sizeof(Pack));
    sendPack.num = 0;
    sendPack.src.pid = myInfo.pid;
    strcpy(sendPack.src.name, myInfo.name);
    write(pfd, &sendPack, sizeof(Pack));

    while (1) {
        memset(&sendPack, 0,sizeof(Pack));
        ret = read(keyfd, buffer, USRNAME_LEN+MAX_DATA_LEN);
        if (ret < 0) {
            if (errno != EAGAIN) {
                perror("read error");
                return -1;
            }
        } else if (ret > 1){
            buffer[ret] = '\0';
            sscanf(buffer, "%s", sendPack.dest.name);
            strcpy(sendPack.data, buffer+strlen(sendPack.dest.name));
            if (strcmp(sendPack.dest.name, "exit") == 0) {
                sendPack.num = 3;
            } else {
                printf("CLIENT:send data: dest user(%s) data:%s", sendPack.dest.name, sendPack.data);
                sendPack.num = 1;
            }
            sendPack.src.pid = myInfo.pid;
            strcpy(sendPack.src.name, myInfo.name);
            write(pfd, &sendPack, sizeof(Pack));
            if (sendPack.num == 3){
                break;
            }
            
        }
        memset(&recvPack, 0,sizeof(Pack));
        ret = read(privfd, &recvPack, sizeof(Pack));
        if (ret < 0) {
            if (errno != EAGAIN) {
                perror("read error");
                return -1;
            }
        } else if (ret > 0){
            switch (recvPack.num) {
            case 1:
                printf("CLIENT:User:%s Pid:%d : %s", recvPack.src.name, recvPack.src.pid, recvPack.data);
                break;
            case 2:
                printf("CLIENT:User:%s is offline!\n", recvPack.src.name);
                break;
            default:
                break;
            }
        }
    }
    close(pfd);
    close(privfd);
    unlink(privFifoName); 
    return 0;
}