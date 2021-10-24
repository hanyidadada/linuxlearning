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
    int num[N];
    pthread_t pid[N];
    
    for (i = 0; i < N; i++) {
        pthread_mutex_init(&(mlock[i]), NULL);
        num[i] = i;
    }
    
    for (i = 0; i < N; i++) {
        pthread_create(&(pid[i]), NULL, eat_thread, &num[i]);
    }
    for (i = 0; i < N; i++) {
        pthread_join(pid[i], NULL);
    }
    return 0;
}

void* eat_thread(void *arg){
    int num = *(int *)arg;
    int ret = 0;
    while(1) {
        ret = pthread_mutex_trylock(&mlock[num % 5]);
        if (ret != 0) {
            printf("\t\tphilosopher %d can't get chopstick %d, give up to eat\n", num + 1, num % 5);
            sleep(3);
            continue;
        }
        printf("\tphilosopher %d holds chopstick %d\n", num + 1, num % 5);
        ret = pthread_mutex_trylock(&mlock[(num + 1) % 5]);
        if (ret != 0) {
            printf("\t\tphilosopher %d can't get chopstick %d, give up to eat, put chopstick %d down\n", num + 1, (num + 1) % 5, num % 5);
            pthread_mutex_unlock(&mlock[num % 5]);
            sleep(3);
            continue;
        }
        printf("\tphilosopher %d holds chopstick %d\n", num + 1, (num + 1) % 5);
        printf("philosopher %d start to eat\n", num + 1);
        sleep(5);
        pthread_mutex_unlock(&mlock[(num + 1) % 5]);
        pthread_mutex_unlock(&mlock[num % 5]);
        printf("philosopher %d finish eating\n", num+1); 
    }

}

