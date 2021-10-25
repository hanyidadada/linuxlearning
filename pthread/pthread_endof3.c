#include <stdio.h>
#include <pthread.h>

void *tfn1(void *arg){
    printf("Thread 1 returning\n");

    return (void *)111;
}

void *tfn2(void *arg){
    printf("Thread 1 exit\n");

    pthread_exit((void *)222);
}

void *tfn3(void *arg){
    while (1)
    {
        // 死循环类的需要设置检查点，主线程才能进行cancel才能杀死子线程
        printf("thread count %d\n", n);
        sleep(2);

        pthread_testcancel(); //自己添加取消点(检查点)
    }
    return (void *)666;
}

int main(void) {
    pthread_t tid;
    int *ret;
    
    pthread_create(&tid, NULL, tfn1, NULL);
    pthread_join(tid, &ret);
    printf("thread 1 return %d\n", (int)ret);
    
    pthread_create(&tid, NULL, tfn2, NULL);
    pthread_join(tid, &ret);
    printf("thread 2 return %d\n", (int)ret);

    pthread_create(&tid, NULL, tfn3, NULL);
    sleep(3);
    pthread_cancel(tid);
    pthread_join(tid, &ret);
    printf("thread 3 return %d\n", (int)ret);
}
