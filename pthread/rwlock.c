#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int counter = 0;
pthread_rwlock_t rwlock;

void* th_write(void *arg){
    int i = (int)arg;
    int t;
    while (1) {
        usleep(9000);
        pthread_rwlock_wrlock(&rwlock);
        t = counter;
        printf("------write %lu: counter = %d ++counter = %d\n", pthread_self(), t, ++counter);
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;

}

void* th_read(void *arg){
    int i = (int)arg;
    while (1) {
        usleep(9000);
        pthread_rwlock_rdlock(&rwlock);
        printf("------read %lu: counter = %d\n", pthread_self(), counter);
        pthread_rwlock_unlock(&rwlock);
        
    }
    return NULL;
}

int main(void){
    int i;
    pthread_t tid[8];

    pthread_rwlock_init(&rwlock, NULL);
    for (i = 0; i < 3; i++) {
        pthread_create(&tid[i], NULL, th_write, (void*)i);
    }
    for (i = 0; i < 5; i++) {
        pthread_create(&tid[i+3], NULL, th_read, (void*)i);
    }
    
    for (i = 0; i < 8; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_rwlock_destroy(&rwlock);
}