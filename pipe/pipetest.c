#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
    pid_t pid;
    int fd[2];
    char buf[14];
    int ret = pipe(fd);
    if (ret < 0) {
        perror("pipe error!");
    }
    pid = fork();
    if (pid < 0) {
        perror("fork error!");
    } else if (pid == 0) { //子进程读
        close(fd[1]); //关闭写
        while(read(fd[0], buf, 14) > 0)
            write(fileno(stdout), buf, 14);
        printf("child recive over!\n");
        exit(0);
    } else { // 父进程写
        close(fd[0]); //关闭读
        write(fd[1], "hello, child!\n", 14);
        printf("    father send over!\n");
        close(fd[1]);
        wait(NULL);
        exit(0);
    }
    return 0;
}
