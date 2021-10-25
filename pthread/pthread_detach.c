#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
void *tfn(void *arg)
{
    int n = 3;
    while (n--)
    {   
        printf("thread count %d\n", n);
        sleep(2);

    }
    return (void *)1;
}

int main(void)
{
    pthread_t tid;
    void *test;
    int err;
#if 1
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, tfn, NULL);
#else
    pthread_create(&tid, NULL, tfn, NULL);
    pthread_detach(tid); // 让线程分离，自动结束，五系统残留资源
#endif

    while (1) {
        err = pthread_join(tid, &test); // 分离了子线程之后，tid将会无效，调用返回参数错误
        printf("------err %d\n",err);
        if (err != 0){
            fprintf(stderr, "pthread_join err: %s\n", strerror(err));
        } else {
            fprintf(stdout, "pthread_join exit code %d\n", (int) test);
        }
        sleep(1);
    }

}