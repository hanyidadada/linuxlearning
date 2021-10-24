#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
    int i;
    pid_t pid;
    
    pid = fork();

    if (pid < 0) {
        printf("fork error\n");
    } else if (pid == 0) {
        printf("\tchild ID = %d\n", getpid());
        printf("\tchild group ID = %d\n", getpgrp());
        sleep(7); //等待父进程修改组id
        printf("-----child group ID changed = %d\n", getpgid(0));
        exit(0);
    } else {
        sleep(1);
        setpgid(pid, pid);
        
        sleep(13);
        printf("\n");
        printf("parent ID = %d\n", getpid());
        printf("parent's parent ID = %d\n", getppid());
        printf("parent group ID = %d\n", getpgid(getpid()));

        sleep(5);
        setpgid(getpid(), getppid());
        printf("parent group ID changed = %d\n", getpgrp());
    }
    return 0;
}
