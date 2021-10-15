#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int n = 0, flag = 0;

void do_sig_parent(int signo)
{
    printf("I'm parent %d\t%d\n", getpid(), n);
    n += 2;
    flag = 1;
    // sleep(1); //确保flag发生变化
}

void do_sig_child(int signo)
{
    printf("I'm child  %d\t%d\n", getpid(), n);
    n += 2;
    flag = 1;
    // sleep(1); //确保flag发生变化
}

int main(void)
{
    pid_t pid;
    struct sigaction act;

    pid = fork();
    if (pid < 0) {
        perror("fork error");
        exit(-1);
    } else if (pid > 0) {
        n = 1;
        sleep(1);
        act.sa_handler = do_sig_parent;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR2, &act, NULL);
        do_sig_parent(0);

        while (1) {
            if (flag == 1) {
                kill(pid, SIGUSR1);
                flag = 0;
            }
        }
    } else {
        n = 2;
        sleep(1);
        act.sa_handler = do_sig_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);
        while (1) {
            if (flag == 1) {
                kill(getppid(), SIGUSR2);
                flag = 0;
            }
        }
    }
    
}

