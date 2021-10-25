#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5

void* eat_thread(void *arg);

pthread_mutex_t mlock[N];

int main(void){
    int i;
    pthread_t pid[N];
    
    for (i = 0; i < N; i++) {
        pthread_mutex_init(&(mlock[i]), NULL);
    }
    
    for (i = 0; i < N; i++) {
        pthread_create(&(pid[i]), NULL, eat_thread, (void *)i);
    }
    for (i = 0; i < N; i++) {
        pthread_join(pid[i], NULL);
    }
    return 0;
}

void* eat_thread(void *arg){
    int num = (int)arg;
    while(1) {
        printf("philosopher %d prepare to eat\n", num + 1);
        pthread_mutex_lock(&mlock[num % N]);
        printf("\tphilosopher %d holds chopstick %d\n", num + 1, num % N);
        pthread_mutex_lock(&mlock[(num + 1) % N]);
        printf("\tphilosopher %d holds chopstick %d\n", num + 1, (num + 1) % N);
        printf("philosopher %d start to eat\n", num + 1);
        sleep(5);
        pthread_mutex_unlock(&mlock[(num + 1) % N]);
        pthread_mutex_unlock(&mlock[num % N]);
        printf("philosopher %d finish eating\n", num+1); 
    }

}

