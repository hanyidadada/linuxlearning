#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char *argv[]){
    int srcfd, dstfd;
    struct stat srcstat, dststat;
    char ch;
    char *srcfilename;
    char destname[128];
    char buffer[128];
    int len;
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
    stat(argv[1], &srcstat);
    if(S_ISDIR(srcstat.st_mode)) {
        perror("src is not a file!");
        close(srcfd);
        exit(-1);
    }
    
    srcfilename = (char *) basename(argv[1]);
    strcpy(destname, argv[2]);
    stat(destname, &dststat);
    if(S_ISDIR(dststat.st_mode)) {
        if (destname[strlen(destname)-1] == '/') {
            strcat(destname, srcfilename);
        } else {
            strcat(destname, "/");
            strcat(destname, srcfilename);
        }
    }
    
    ret = access(destname, F_OK);
    if (ret == 0) {
        printf("source file is already exit! should I overwrite or append it?(o(verwerite) or a(ppend) or n(o)):");
        scanf(" %c", &ch);
        if(ch == 'n') {
            exit(0);
        } else if (ch == 'o') {
            dstfd = open(destname, O_RDWR|O_TRUNC, 0644);
        } else if (ch == 'a') {
            dstfd = open(destname, O_RDWR|O_APPEND, 0644);
        } else {
            printf("input error!\n");
            close(srcfd);
            exit(0);
        }
    } else {
        dstfd = open(destname, O_RDWR|O_CREAT, 0644);
    }
    
    if (dstfd < 0) {
        perror("open destfile wrong!");
        close(srcfd);
        exit(-1);
    }
    while(1) {
        memset(buffer, 0, 128);
        len = read(srcfd, buffer, 127);
        write(dstfd, buffer, 127);
        if (len < 127) {
            break;
        }
    }

    close(dstfd);
    close(srcfd);
    return 0;
}
