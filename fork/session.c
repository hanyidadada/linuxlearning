#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(-1);
    } else if (pid == 0) {
        printf("\tchild ID = %d\n", getpid());
        printf("\tchild group ID = %d\n", getpgrp());
        printf("\tchild session ID = %d\n", getsid(0));

        sleep(5);
        setsid();
        printf("changed:\n");
        printf("\tchild ID = %d\n", getpid());
        printf("\tchild group ID = %d\n", getpgrp());
        printf("\tchild session ID = %d\n", getsid(0));
        sleep(12);
    }
    return 0;
    
}