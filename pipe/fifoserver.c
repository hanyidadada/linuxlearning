#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FIFO_NAME "MyFifo"

int main(void){
    int fifofd, ret, bufferlen;
    char buffer[128];

    unlink(FIFO_NAME); // 删除之前的同名管道文件
    ret = mkfifo(FIFO_NAME, 0777);
    if (ret < 0) {
        perror("mkfifo error!");
        exit(1);
    }

    fifofd = open(FIFO_NAME, O_RDONLY);
    if (fifofd < 0) {
        perror("open fifo error!");
        exit(2);
    }
    
    do{
        memset(buffer, 0, 21);
        bufferlen = read(fifofd, buffer, 20);
        if (bufferlen < 0) {
            perror("read FIFO error!");
        }
        buffer[bufferlen] = '\0';
        printf("Client Message: %s\n", buffer);
    }while(memcmp("close", buffer, 5));
    close(fifofd);
    unlink(FIFO_NAME);
    return 0;
}





