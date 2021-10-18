#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char *argv[]){
    int srcfd, dstfd;
    struct stat srcstat, dststat;
    char ch;
    char *srcfilename;
    char destname[128];
    char *srcbuffer, *dstbuffer;
    pid_t pid;
    int n = 5;
    int size, ret, i; 
    if (argc < 3) {
        perror("./run sourcfile dest_diretory");
        exit(-1);
    }
    
    ret = access(argv[1], F_OK);
    if (ret < 0) {
        perror("source file doesn't exit");
        exit(-1);
    } 

    srcfd = open(argv[1], O_RDONLY);
    if (srcfd < 0) {
        perror("open sourcefile wrong!");
        exit(-1);
    }
    ret = fstat(srcfd, &srcstat);
    if (ret < 0) {
        perror("fstat wrong!");
        exit(-1);
    }
    srcbuffer = mmap(NULL, srcstat.st_size, PROT_READ, MAP_SHARED, srcfd, 0);
    if (srcbuffer == MAP_FAILED) {
        perror("mmap src file failed:");
        exit(-1);
    }
    close(srcfd);
    
    srcfilename = (char *) basename(argv[1]);
    strcpy(destname, argv[2]);
    stat(destname, &dststat);
    if(S_ISDIR(dststat.st_mode)) {
        printf("dest is dirctory\n");
        if (destname[strlen(destname)-1] == '/') {
            strcat(destname, srcfilename);
        } else {
            strcat(destname, "/");
            strcat(destname, srcfilename);
        }
    }
    
    ret = access(destname, F_OK);
    if (ret == 0) {
        printf("source file is already exit! should I overwrite it?(y or n)");
        scanf(" %c", &ch);
        if(ch == 'n') {
            exit(0);
        }
    }
    dstfd = open(destname, O_RDWR|O_CREAT, 0644);
    if (dstfd < 0) {
        perror("open destfile wrong!");
        exit(-1);
    }
    ftruncate(dstfd, srcstat.st_size);
    dstbuffer = mmap(NULL, srcstat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, srcfd, 0);
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
            size = srcstat.st_size / n;
            if (i == n-1) {
                memcpy(dstbuffer + i * size, srcbuffer+i * size, size + (srcstat.st_size % n));
            } else {
                memcpy(dstbuffer + i * size, srcbuffer+i * size, size);
            }
            exit(0);;
        } else {
            continue;
        }
    }
    for (i = 0; i < n; i++) {
        wait(NULL);
    }
    printf("copy finished!\n");
    munmap(dstbuffer, srcstat.st_size);
    munmap(srcbuffer, srcstat.st_size);
    exit(0);
}


