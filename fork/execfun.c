#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid;
    int status;
    char* argv[]={"ls", "-a", "-l", NULL};
    printf("This is father progress!Id:%u\n", getpid());
    pid = fork();
    if (pid < 0) {
        perror("Create progress error!\n"); 
    } else if (pid == 0) {
        // execlp("ls", "ls", "-l", "-a", NULL);    
        execv("/home/ywj/fork/test", argv); 
        // execl("/home/ywj/fork/test", "./test", NULL); 
        // execl("/bin/ls", "ls", "-l", "-a", NULL);    
    } else {
        printf("Father progress!\n");
	    waitpid(-1, &status, 0);
    }
    return 0;
}
