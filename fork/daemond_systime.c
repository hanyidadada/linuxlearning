#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void daemon_init(void){
    int fd;
    pid_t pid, sid;
    time_t t;
    char buffer[1024];
    pid = fork();
    if (pid < 0){
        perror("fork error");
        exit(-1);
    } else if (pid > 0) {
        exit(0);
    }

    sid = setsid();
    chdir("/home/ywj");

    umask(0002);
    fd = open("/home/ywj/ywj.txt", O_RDWR|O_CREAT, 0644);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    while(1){
        time(&t);   
        printf("%s", ctime(&t));
        sleep(1);
    }

}

int main(void){
    daemon_init();
}