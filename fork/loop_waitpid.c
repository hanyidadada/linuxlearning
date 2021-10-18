#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid[5];
    int i, wpid;
    int status;
    printf("This is father progress!Id:%u\n", getpid());
    for(i = 0; i < 5; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("Create progress error!\n"); 
        } else if (pid[i] == 0) {
            sleep(1);
        	printf("This is child %d!\n", i+1);
            break;
        } else {
           printf("Father creates %d progress!\n", i+1);
        }
    }
    if (i == 5) {
        do {
            wpid = waitpid(-1, NULL, 0);
            if (wpid > 0) {
                i--;
            }
        }while (i > 0);
        printf("Father end!\n");
    }
    return 0;
}
