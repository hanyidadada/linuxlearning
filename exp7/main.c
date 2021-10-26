#include "header.h"
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int cnum, pnum, i, semid;
    char argment[2];
    if(argc < 3) {
        perror("too few argment(./run num1 num2)");
        return -1;
    }

    sscanf(argv[1], "%d", &pnum);
    sscanf(argv[2], "%d", &cnum);
    init_allsem(&semid);
    //创建生产者进程
    for (i = 0; i < pnum; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork error");
            return -1;
        } else if (pid == 0) {
            memset(argment, 0, 2);
            sprintf(argment, "%d", i+1);
            execlp("/root/linuxexp/exp7/producer", "producer", argment, NULL);
            return 0;
        } else {
            continue;
        }
    }

    //创建消费者进程
    for (i = 0; i < cnum; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork error");
            return -1;
        } else if (pid == 0) {
            memset(argment, 0, 2);
            sprintf(argment, "%d", i+1);
            execlp("/root/linuxexp/exp7/consumer", "consumer", argment, NULL);
            return 0;
        } else {
            continue;
        }
    }
    for (i = 0; i < pnum + cnum; i++) {
        wait(NULL);
    }

    del_sem(semid, SEM_FULL);
    printf("All ending\n");
    return 0;
}
