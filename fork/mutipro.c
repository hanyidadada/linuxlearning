#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void) {
    pid_t pid;
    int i;
    int status;
    printf("This is father progress!Id:%u\n", getpid());
    for(i = 0; i < 5; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Create progress error!\n"); 
        } else if (pid == 0) {
            sleep(1);
        	printf("This is child %d!\n", i+1);
            break;
        } else {
           printf("Father creates %d progress!\n", i+1);
        }
    }
    if (i == 5) {
        sleep(4);
        printf("Father end!\n");
    }
    return 0;
}
