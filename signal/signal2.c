#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void stopfun(int signo)
{
    printf("recive stop signal and sleep 5s!\n");
    sleep(5);
    printf("stop signal sleep over!\n");
    
}

void myfun(int signo)
{
    printf("recive quit signal and sleep 5s!\n");
    sleep(5);
    printf("quit signal sleep over!\n");
}


int main(void)
{
    int ret;
    struct sigaction act, oldact;
    act.sa_flags = 0;
    act.sa_handler = myfun;
    sigemptyset(&(act.sa_mask));
    sigaddset(&(act.sa_mask), SIGINT);
    sigaddset(&(act.sa_mask), SIGTSTP);

    ret = sigaction(SIGQUIT, &act, &oldact);
    if (ret < 0) {
        perror("signal quit error");
        exit(-1);
    }
    
    sigemptyset(&(act.sa_mask));
    sigaddset(&(act.sa_mask), SIGINT);
    sigaddset(&(act.sa_mask), SIGQUIT);
    act.sa_handler = stopfun;
    ret = sigaction(SIGTSTP, &act, &oldact);
    if (ret < 0) {
        perror("signal quit error");
        exit(-1);
    }
    while(1); 
}
