#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    pid_t pid;
    pid = fork();

    if(pid < 0){
        perror("create child error\n");
        return 0;
    } else if (pid == 0){
        printf("---child, my partent = %d, going to sleep 10s.\n", getppid());
        sleep(10);
        printf("---------------child die---------------\n");
    } else {
        while(1){
            printf("Parent, pid = %d , child = %d\n", getpid(), pid);
            sleep(1);
        }
    }
    return 0;
}
