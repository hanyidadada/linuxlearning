#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

int main(void) {
    char *addr = NULL;
    int ret, len = 4;
    int fd = open("mytest.txt", O_CREAT|O_RDWR, 0644);
    if (fd < 0) {
        perror("oppen error");
        exit(1);
    }
    ret = ftruncate(fd, len);
    if (ret < 0) {
        perror("ftruncate error!");
        exit(2);
    }
    addr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);// 包含一次读操作
    if (addr == MAP_FAILED) {
        perror("mmap error!");
        exit(3);   
    }
    strcpy(addr, "abc");
    close(fd);
    ret = munmap(addr, len);
    if (ret < 0) {
        perror("munmap error!");
        exit(4);
    } 
    return 0;
}

