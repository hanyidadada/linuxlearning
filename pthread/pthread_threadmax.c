#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *tfn(void *arg){
    while (1) {
        sleep(1);
    }
    
}

int main(void)
{
    pthread_t tid;
    int i, ret;
    for (i = 1; 1; i++) {
        ret = pthread_create(&tid, NULL, tfn, NULL);
        if(ret != 0) {
            fprintf(stderr, "pthread_create err: %s\n", strerror(ret));
            return 0;
        }
         printf("------thread %d\n", i);
    }
    
}
