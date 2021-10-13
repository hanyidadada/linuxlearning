#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

void myfun(int signo)
{
    printf("Hello world!\n");
}

void quitfun(int signo)
{
    printf("Bye Bye\n");
    exit(0);
}

void printsig(sigset_t *set)
{
    int i;
    for(i = 1; i < 64; i++)
    {
        if(sigismember(set,i)){
            putchar('1');
        } else {
            putchar('0');
        }
    }
    printf("\n");
}

int main(void){
    sigset_t myset, oldset, ped;
    struct itimerval newval, oldval;

    sigemptyset(&myset);
    sigaddset(&myset, SIGALRM);
    
    memset(&newval, 0, sizeof(struct itimerval));
    memset(&oldval, 0, sizeof(struct itimerval));
    newval.it_value.tv_sec = 3;     //第一次触发时间
    newval.it_interval.tv_sec = 2; // 下一次触发时间
    signal(SIGALRM, myfun);
    signal(SIGQUIT, quitfun);
    setitimer(ITIMER_REAL, &newval, &oldval);

    while(1){
        sleep(5);
        if (rand() % 2) {
            sigprocmask(SIG_BLOCK, &myset, &oldset);
            printf("\tNow SIGALRM set SIG_BLOCK\n");
        } else {
            sigprocmask(SIG_UNBLOCK, &myset, &oldset);
            printf("\tNow SIGALRM set SIG_UNBLOCK\n");
        }
        sigpending(&ped);
        printsig(&ped);
    }

    return 0;
}