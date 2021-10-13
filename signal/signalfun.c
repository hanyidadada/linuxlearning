#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>

void myfun(int signo)
{
    printf("Hello world!\n");
}

void quitfun(int signo)
{
    printf("Bye Bye\n");
    exit(0);
}

int main(void) {
    struct itimerval newval, oldval;
    memset(&newval, 0, sizeof(struct itimerval));
    memset(&oldval, 0, sizeof(struct itimerval));
    newval.it_value.tv_sec = 3;     //第一次触发时间
    newval.it_interval.tv_sec = 2; // 下一次触发时间
    signal(SIGALRM, myfun);
    signal(SIGQUIT, quitfun);
    setitimer(ITIMER_REAL, &newval, &oldval);

    while(1);

    return 0;
}