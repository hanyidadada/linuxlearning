#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FIFO_NAME "MyFifo"

int main(void){
    int fifofd, ret;
    char buffer[21];
    int bufferlen;

    ret = access(FIFO_NAME, F_OK);
    if (ret < 0) {
        perror("FIFO is not ok!");
        exit(1);
    }
    fifofd = open(FIFO_NAME, O_WRONLY);
    if (fifofd < 0) {
        perror("open fifo error!");
        exit(2);
    }
    
    do{
        memset(buffer, 0, 21);
        printf("Input to server:");
        scanf("%s", buffer);
        write(fifofd, buffer, strlen(buffer));
        printf("write: %s\n", buffer);
    }while(memcmp("close", buffer, 5));
    return 0;
}

