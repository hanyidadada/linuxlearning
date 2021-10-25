#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    int a;
    int b;
}exit_t;


void* thread_fun(void *arg)
{
    //返回结构体只能使用malloc分配，不可以返回局部变量取地址
    exit_t *ret;
    ret = (exit_t*)malloc(sizeof(exit_t));
    ret->a = 10;
    ret->b = 20;
    // 设置返回值，既可以使用return 也可以pthread_exit
    pthread_exit((void *)ret);
    // return (void *)ret;
}

int main(void) 
{
    pthread_t tid;
    exit_t *ret;
    pthread_create(&tid, NULL, thread_fun, NULL);

    pthread_join(tid, (void **)&ret);

    printf("a = %d, b = %d\n", ret->a, ret->b);
    free(ret);
    return 0;
}