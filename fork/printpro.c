#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void){
    pid_t pid = 0;
    int status;
    int file = open("programinfo", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    pid = fork();
    if(pid == 0) {
        dup2(file, fileno(stdout));
        execlp("ps", "ps", "aux", NULL);
        perror("error");
    } else if (pid > 0) {
        close(file);
        waitpid(-1, &status, 0);
    } else {
        printf("create child error!");
    }
}
