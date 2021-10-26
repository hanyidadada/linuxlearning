#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

int main(int argc, char *argv[]){
    int fd;
    struct flock lock;

    if (argc < 2) {
        printf("argc err! ./run filename\n");
        return -1;
    }
    
    if ((fd = open(argv[1], O_RDWR)) < 0){
        printf("open file error:%s\n", strerror(errno));
        return -1;
    }
    
    // lock.l_type = F_RDLCK; // 读锁共享，可以多个同时申请读锁
    lock.l_type = F_WRLCK;  //写锁互斥，因此不可同时申请成功，另一个会阻塞
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    
    fcntl(fd, F_SETLKW, &lock);
    printf("get rwlock\n");
    
    sleep(10);

    lock.l_type = F_ULOCK;
    fcntl(fd, F_SETLKW, &lock);
    printf("release lock\n");
    return 0;
}