#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 0X1000000

void *tfn(void *arg) {
    while (1) {
        sleep(1);
    }
}

int main(void){
    pthread_t tid;
    int err, detachstate, i = 1;
    pthread_attr_t attr;
    size_t stacksize;
    void *stackaddr;

    pthread_attr_init(&attr);
    pthread_attr_getstack(&attr, &stackaddr, &stacksize);
    pthread_attr_getdetachstate(&attr, &detachstate);

    if (detachstate == PTHREAD_CREATE_DETACHED) {
        printf("thread detch\n");
    } else if (detachstate == PTHREAD_CREATE_JOINABLE) {
        printf("thread join\n");
    } else {
        printf("thread unknown\n");
    }

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while (1) {
        stackaddr = malloc(SIZE);
        if (stackaddr == NULL) {
            perror("malloc");
            exit(-1);
        }
        stacksize = SIZE;
        pthread_attr_setstack(&attr, stackaddr, stacksize);

        err = pthread_create(&tid, &attr, tfn, NULL);
        if (err != 0){
            printf("%s\n", strerror(err));
            break;
        }
        printf("%d\n", i++);
    }
    pthread_attr_destroy(&attr);
    return 0;
}