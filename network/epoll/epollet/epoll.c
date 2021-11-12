#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

#define MAXNUM 10

int main(void){
    pid_t pid;
    int fd[2];
    char buffer[MAXNUM];
    pipe(fd);
    pid = fork();
    if (pid == 0) {
        close(fd[0]);
        char ch = 'a';
        int i;
        while (1) {
            for (i = 0; i < MAXNUM / 2; i++) {
                buffer[i] = ch;
            }
            buffer[i - 1] = '\n';
            ch ++;
            for (; i < MAXNUM; i++) {
                buffer[i] = ch;
            }
            buffer[i - 1] = '\n';
            ch ++;
            write(fd[1], buffer, MAXNUM);
            sleep(5);
        }
        close(fd[1]);
    } else if (pid > 0) {
        close(fd[1]);
        struct epoll_event tmp, events;
        int epfd;
        tmp.data.fd = fd[0];
        tmp.events = EPOLLIN | EPOLLET; //ET边沿触发
        // tmp.events = EPOLLIN; //默认LT水平触发
        epfd = epoll_create(1);
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd[0], &tmp);
        while (1) {
            epoll_wait(epfd, &events, 1, 0); //设置为阻塞，部分设备调度陷入永久阻塞不再继续运行
            if (events.data.fd == fd[0]) {
                memset(buffer, 0, MAXNUM);
                read(fd[0], buffer, MAXNUM/2);
                printf("get from pipe:%s", buffer);
            }
        }
        
    } else {
        perror("fork error");
        return -1;
    }
    
    return 0;
}