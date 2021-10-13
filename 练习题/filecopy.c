#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
    int srcfd, dstfd;
    struct stat buffer;
    char *srcbuffer, *dstbuffer;
    pid_t pid;
    int n = 5;
    int size, ret, i; 
    if (argc < 4) {
        perror("./run sourcfile dest_diretory num");
        exit(-1);
    }
    sscanf(argv[3], "%d", &n);
    srcfd = open(argv[1], O_RDONLY);
    if (srcfd < 0) {
        perror("open sourcefile wrong!");
        exit(-1);
    }
    ret = fstat(srcfd, &buffer);
    if (ret < 0) {
        perror("fstat wrong!");
        exit(-1);
    }
    srcbuffer = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, srcfd, 0);
    if (srcbuffer == MAP_FAILED) {
        perror("mmap src file failed:");
        exit(-1);
    }
    close(srcfd);

    dstfd = open(argv[2], O_RDWR|O_CREAT, 0644);
    if (dstfd < 0) {
        perror("open sourcefile wrong!");
        exit(-1);
    }
    ftruncate(dstfd, buffer.st_size);
    dstbuffer = mmap(NULL, buffer.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, srcfd, 0);
    if (dstbuffer == MAP_FAILED) {
        perror("mmap src file failed:");
        exit(-1);
    }
    close(dstfd);
    printf("copy start!\n");
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork error:");
            exit(-1);
        } else if (pid == 0) {
            size = buffer.st_size / n;
            if (i == n-1) {
                memcpy(dstbuffer + i * size, srcbuffer+i * size, size + (buffer.st_size % n));
            } else {
                memcpy(dstbuffer + i * size, srcbuffer+i * size, size);
            }
            exit(0);
        } else {
            continue;
        }
    }
    for (i = 0; i < n; i++) {
        wait(NULL);
    }
    printf("copy finished!\n");
    munmap(dstbuffer, buffer.st_size);
    munmap(srcbuffer, buffer.st_size);
    exit(0);
}


