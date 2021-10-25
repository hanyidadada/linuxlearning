#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM 5

int var = 100;

void *thread_fun(void *arg){
    int i = (int)arg;
    sleep(i);
    if (i == 1) {
        var = 333;
        printf("var = %d\n", var);
        pthread_exit((void*)var);
    } else if (i == 3) {
        var = 777;
        printf("I'm %dth pthread, pthread_id = %lu, var = %d\n", i, pthread_self(),var);
        pthread_exit((void*)var);
    } else {
        printf("I'm %dth pthread, pthread_id = %lu, var = %d\n", i, pthread_self(), var);
        pthread_exit((void*)var);
    }
}

int main(void){
    pthread_t tid[NUM];
    int *ret, i;
    for (i = 0; i < NUM; i++) {
        pthread_create(&tid[i], NULL, thread_fun, (void *)i);
    }

    for (i = 0; i < NUM; i++){
        pthread_join(tid[i], (void**)&ret);
        printf("----child %d ret %d\n", i, (int)ret);
    }
    
    printf("I'm main thread tid = %lu, var = %d\n", pthread_self(), var);

    return 0;
}