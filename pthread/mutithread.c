#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NUM 5

void* thread_fun(void *arg)
{
    sleep(1);
    printf("\t thread i:%d", (int)arg); //传入的直接就是整数，非地址指针
    printf(" thread id = %lu, pid = %d  run success\n", pthread_self(), getpid());
    // 测试子线程中exit(), 会退出整个进程
    // if ((int)arg == 3) {
    //     exit(0);
    // }
    
    pthread_exit(NULL); // 退出当前调用的子线程
}

int main(void) {
    pthread_t tid[NUM];
    int ret, i;
    
    for (i = 0; i < NUM; i++) {
        ret = pthread_create(&tid[i], NULL, thread_fun, (void*)i); // 不可以传入i的指针，子线程调用的时候i地址所指向的值发生改变，无法确定，不能被子线程正确引用程序会出错
        if (ret != 0){
            fprintf(stderr ,"pthread_create error:%s\n", strerror(ret));
            exit(-1);
        }
    }
    pthread_exit(NULL); // main中仅退出main主线程，子线程仍然继续运行，子线程全部退出才会使得进程退出
    // for (i = 0; i < NUM; i++) {
    //     pthread_join(tid[i], NULL);
    // }
    
}