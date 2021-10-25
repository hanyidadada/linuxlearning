#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// 线程之间共享全局变量，进程之间不共享
int global;
int global_init = 12;

void* thread_fun(void *arg)
{
    printf("\tthread id = %lu, pid = %d  run success\n", pthread_self(), getpid());
    printf("\tglobal: %d, global_init: %d\n", global, global_init);
    printf("\tnow change\n");
    global = 111;
    global_init = 222;
    printf("\tglobal: %d, global_init: %d\n", global, global_init);
}

int main(void) {
    pthread_t tid;
    int ret;
    printf("Before child thread\n");
    printf("global: %d, global_init: %d\n", global, global_init);
    ret = pthread_create(&tid, NULL, thread_fun, NULL);
    if (ret != 0){
        fprintf(stderr ,"pthread_create error:%s\n", strerror(ret));
        exit(-1);
    }
    printf("main thread: child id %lu\n", tid);
    printf("main threadid = %lu, pid = %d\n", pthread_self(), getpid());
    pthread_join(tid, NULL);
    printf("after child pthread\n\tglobal: %d, global_init: %d\n", global, global_init);
    return 0;
}