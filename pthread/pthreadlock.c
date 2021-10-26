#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;

void* tfn(void *arg)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        printf("hello ");
        sleep(rand()%3);
        printf("world\n");
        pthread_mutex_unlock(&mutex);
        sleep(rand()%3);
    }
    
}

int main(void){
    pthread_t tid;
    int flag = 5;
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tid, NULL, tfn, NULL);
    while (flag--) {
        pthread_mutex_lock(&mutex);
        printf("HELLO ");
        sleep(rand()%3);
        printf("WORLD\n");
        pthread_mutex_unlock(&mutex);
        sleep(rand()%3);
    }
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    pthread_mutex_destroy(&mutex);
}