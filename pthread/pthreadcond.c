#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct msg {
    struct msg *next;
    int num;
};

struct msg *head;

// 静态初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* producer(void *arg){
    struct msg *mp;
    while (1) {
        mp = NULL;
        mp = (struct msg *)malloc(sizeof(struct msg));
        mp->num = rand()%1000 + 1;
        pthread_mutex_lock(&mutex);
        mp->next = head;
        head = mp;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        printf("--producer %d---%d\n", ((int)arg) + 1, mp->num);
        sleep(rand()%5);
    }
    
}

void* consumer(void *arg){
    struct msg *mp;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (head == NULL) {
            pthread_cond_wait(&cond, &mutex);
        }
        mp = head;
        head = head->next;
        pthread_mutex_unlock(&mutex);
        printf("--Consume %d---%d\n", ((int)arg) + 1, mp->num);
        free(mp);
        sleep(rand()%5);
    }
    
}

int main(void){
    pthread_t pid[3], cid[5];
    int i;
    for (i = 0; i < 3; i++) {
        pthread_create(&pid[i], NULL, producer, (void*)i);
    }
    
    for (i = 0; i < 5; i++) {
        pthread_create(&cid[i], NULL, consumer, (void*)i);
    }
    
    for (i = 0; i < 3; i++) {
        pthread_join(pid[i], NULL);
    }
    
    for (i = 0; i < 5; i++) {
        pthread_join(cid[i], NULL);
    }    
    return 0;
}
