#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
// 未能捕获，个人猜测时exec函数会覆盖原来代码，注册了后执行exec没有用

void do_child_sig(int signum)
{
    pid_t pid;
    while(pid = waitpid(0, NULL, WNOHANG) > 0) {

    }
    printf("catch child %d\n", pid);
}

int main(void) {
    pid_t pid;
    int fd[2];
    pipe(fd);

    pid = fork();
    signal(SIGCHLD, do_child_sig);

    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execlp("ls", "ls", NULL);
    } else if (pid > 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL);
    }
    return 0;
}