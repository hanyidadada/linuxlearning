#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
    int i;
    pid_t pid;

    for (i = 0; i < 10; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork error");
            exit(-1);
        } else if (pid == 0) {
            break;
        } else {
            // setpgid(pid, pid);
        }
    }
    
    if (pid == 0) {
        while(1) {
            printf("child progress %d:%d\n", getpgrp(), getpid());
            sleep(2);
        }
    }
    
    if (pid > 0) {
        pid_t cpid;
        while((cpid = wait(NULL)) > 0){
            printf("Kill child %d\n", cpid);
        }
    }
    return 0;
}