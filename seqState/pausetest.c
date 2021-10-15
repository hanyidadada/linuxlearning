#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void sigfun(int signo) {
    printf("child catch signal\n");
}

int main(void) {
    pid_t pid;
    int ret;
    pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(-1);
    } else if (pid == 0) {
        signal(SIGCONT, sigfun);
        printf("child pause\n");
        ret = pause();
        if (ret < 0) {
            printf("child restart!\n");
            exit(0);
        }
    } else {
        sleep(1);
        printf("father send signal to child\n");
        kill(pid, SIGCONT);
        wait(NULL);
    }
    return 0;
}
