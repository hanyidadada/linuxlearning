#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/mman.h>

struct mp{
    int num;
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
};

int main(void){
    struct mp* mm;
    int i;
    pid_t pid;

    mm = mmap(NULL, sizeof(struct mp), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    pthread_mutexattr_init(&(mm->attr));
    pthread_mutexattr_setpshared(&(mm->attr), PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(mm->mutex), &(mm->attr));

    pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(-1);
    } else if (pid == 0) {
        for (i = 0; i < 10; i++) {
            pthread_mutex_lock(&(mm->mutex));
            mm->num++;
            printf("-----child num++ %d\n", mm->num);
            pthread_mutex_unlock(&(mm->mutex));
            sleep(rand() % 3);
        }
        return 0;
    } else {
        for (i = 0; i < 10; i++) {
            sleep(rand() % 3);
            pthread_mutex_lock(&(mm->mutex));
            mm->num += 2;
            printf("-----parent num+2 %d\n", mm->num);
            pthread_mutex_unlock(&(mm->mutex));
        }
        wait(NULL);
    }
    
    pthread_mutexattr_destroy(&(mm->attr));
    pthread_mutex_destroy(&(mm->mutex));
    munmap((void *)mm, sizeof(struct mp));
    return 0;
}