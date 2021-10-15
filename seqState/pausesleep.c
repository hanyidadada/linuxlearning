#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

extern int errno;

void sigfun(int signo) {
    return ;
}

int sleepfun(int num){
    int ret;
    struct sigaction act, oldact;
    sigset_t newmask, oldmask, susmask;

    printf("\tstart to sleep!\n");
    ret = alarm(num);
    if (ret < 0) {
        perror("alarm error");
        exit(-1);
    }
    act.sa_handler = sigfun;
    sigaction(SIGALRM, &act, &oldact);
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask); //屏蔽alarm
    
    alarm(num);

    susmask = oldmask;
    sigdelset(&susmask, SIGALRM); //解除alarm屏蔽
    ret = sigsuspend(&susmask);
    //ret = pause(); // 分开pause和解除屏蔽会产生时序问题
    if (ret < 0) {
        if (errno == EINTR) {
            printf("\tend sleep!\n");
        }
    }
    alarm(0);
    sigaction(SIGALRM, &oldact, NULL);
    sigprocmask(SIG_SETMASK, &oldmask, NULL); // 恢复屏蔽字
    return 0;
}

int main(void) {
    
    printf("Test for sleep func\n");
    while (1) {
        printf("-----------\n");
        sleepfun(5);
        printf("\n");
    }
    return 0;
}
