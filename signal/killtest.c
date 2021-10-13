#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

int main(void) {
    int ret, i, j;
    pid_t pid;
    for (i = 0; i < 5; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork error");
            return -1;
        } else if (pid == 0) {
            for (i = 0; i < 5; i++) {
                printf("child %d live\n", getpid());
            }
            printf("child %d exit normall\n", getpid());
            return 0;
        } else {
            if (i == 3) {

                ret = kill(pid, SIGKILL);
                if (ret < 0) {
                    perror("kill child error!");
                    return -1;
                }
                printf("killed %d\n", pid);
            }
        }        
    }
    ret = kill(getpid(), SIGKILL);
    if (ret < 0) {
        perror("kill self error!");
        return -1;
    } 
}
