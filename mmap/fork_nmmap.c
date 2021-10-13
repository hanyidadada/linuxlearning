#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
int var = 100;

int main(void){
    int ret;
    pid_t pid;
    int *addr;
    // int fd = open("/dev/zero", O_RDWR);

    // addr = (int *)mmap(NULL, 20, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    addr = (int *)mmap(NULL, 4, PROT_WRITE|PROT_READ, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed!");
        exit(3);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork error:");
        exit(4);
    } else if (pid == 0) {
        *addr = 1000;
        var = 200;
        printf("child, *addr = %d, var = %d\n", *addr, var);
    } else {
        sleep(1);
        printf("father, *addr = %d, var = %d\n", *addr, var);
        wait(NULL);

        ret = munmap(addr, 20);
        if (ret < 0) {
            perror("munmap error:");
            exit(5);
        }
    }
    return 0;      
}
