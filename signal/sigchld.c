#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void do_child(int signo)
{
    int status;
    pid_t pid;
    while(pid = waitpid(0, &status, WNOHANG) > 0) {
        if (WIFEXITED(status)) {
            printf("----child %d exit %d----\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("----child %d signal %d----\n", pid, WTERMSIG(status));
        }
    }
}

int main(void){
    pid_t pid;
    int i;
    sigset_t mask, old;
    for (i = 0; i < 10; i++) {
        pid = fork();
        if (pid < 0) {
            perror("create child err:");
            exit(-1);
        } else if (pid == 0) {
            break;
        }
               
    }
    if (pid == 0) {
        int n =1;
        while(n--) {
            printf("child ID %d\n", getpid());
            sleep(1);
        }
        return i+1;
    } else if (pid > 0) {
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, &old);
        struct sigaction act;
        act.sa_handler = do_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGCHLD, &act, NULL);
        sigprocmask(SIG_SETMASK, &old, NULL);
        while(1) {
            printf("parent ID %d\n", getpid());
            sleep(1);
        }
    }
    
    return 0;
}