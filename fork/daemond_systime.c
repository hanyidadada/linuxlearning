#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void daemon_init(void){
    pid_t pid, sid;

    pid = fork();
    if (pid < 0){
        perror("fork error");
        exit(-1);
    } else if (/* condition */)
    {
        /* code */
    }
    
    

}