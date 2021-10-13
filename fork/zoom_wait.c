#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    pid_t pid;
    int re;
    int status;
    pid = fork();

    if(pid < 0){
        perror("create child error\n");
        return 0;
    } else if (pid == 0){
        printf("---child, my partent = %d, going to sleep 3s.\n", getppid());
        sleep(20);
        printf("---------------child die---------------\n");
        exit (1);
    } else {
        re = wait(&status);
        if (re == -1) {
            perror("wait error");
            exit(1);
        } 
        if (WIFEXITED(status)) {
            printf("child exit status: %d\n", WEXITSTATUS(status));
        } 
        if (WIFSIGNALED(status)) {
            printf("child exit signal num: %d\n", WTERMSIG(status));
        } 
        while(1) {
            printf("Parent, pid = %d , child = %d\n", getpid(), pid);
            sleep(1);
        }
    }
    return 0;
}
