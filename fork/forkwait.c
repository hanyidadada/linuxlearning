#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(void){
    int n = 3;
    pid_t pid; 
    int wpid, i, status;
    for(i = 0; i < n; i++){
        pid = fork();
        if (pid < 0){
            perror("create child error!");
        } else if (pid ==0){
            if (i == 0){
                execlp("ps", "ps", NULL);
                exit(0);
            } else if (i == 1) {
                execl("/home/ywj/fork/printpro", "printpro", NULL);
                exit(0);
            } else {
                execl("/home/ywj/fork/wrong", "wrong", NULL);
                exit(0);
            }
        } else {
            printf("Father create %d child, id = %d\n", i, pid);
        }
    }
    do{
        wpid = waitpid(-1, &status, WNOHANG);
        if (wpid > 0) {
            i--;
            if (WIFEXITED(status)) {
                printf("child exit normal. id = %d, status = %d\n", wpid, WEXITSTATUS(status));
            }
            if (WIFSIGNALED(status)) {
                printf("child exit unnormal. id = %d, siganl = %d\n", wpid, WTERMSIG(status));
            }
            printf("father killed child %d\n", wpid);
        }
    }while (i > 0);
    return 0;
}
