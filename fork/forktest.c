#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void){
    pid_t pid;
    
    pid = fork();
    if (pid < 0){
        perror("create error!\n");
        exit(1);
    } else if (pid > 0) {
        sleep(1);
        printf("Parent pid = %u, parentId = %u\n", getpid(), getppid());
    } else{
        printf("Child pid = %u, parentId = %u\n", getpid(), getppid());
        sleep(3);
        printf("Child pid = %u, parentId = %u\n", getpid(), getppid());
    }

}
