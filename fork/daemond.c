#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void my_daemon(void){
    pid_t pid, sid;
    int ret;
    pid = fork();  //父进程调用fork并退出
    if (pid < 0) {
        perror("fork error");
        exit(-1);
    } else if (pid > 0) {
        exit(0);
    }
    // 子进程创建会话
    sid = setsid();
    if (sid < 0) {
        perror("setsid error");
        exit(-2);
    }
    // 子进程修改当前工作目录
    ret = chdir("/home/ywj");
    if (ret < 0) {
        perror("chdir error");
        exit(-3);
    }
    //设置文件创建umask
    umask(0002);

    close(STDIN_FILENO);
    open("/dev/null", O_RDWR);
    dup2(0, STDOUT_FILENO);
    dup2(0, STDERR_FILENO);
}

int main(void)
{
    my_daemon();

    while(1){
        
    }
    return 0;
}