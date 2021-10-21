#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

struct thread_arg{
    char workdir[128];
    char destdir[128];
};

void* copyfile_thread(void *arg){
    int srcfd, dstfd;
    struct stat dststat;
    char ch;
    char *srcfilename;
    char destname[128];
    char buffer[128];
    struct thread_arg *argv = (struct thread_arg *)arg;
    int len, ret;

    ret = access(argv->workdir, F_OK);
    if (ret < 0) {
        printf("%s\n", argv->workdir);
        perror("source file doesn't exit");
        pthread_exit(NULL);
    } 

    srcfd = open(argv->workdir, O_RDONLY);
    if (srcfd < 0) {
        perror("open sourcefile wrong!");
        pthread_exit(NULL);
    }
    
    srcfilename = (char *) basename(argv->workdir);
    strcpy(destname, argv->destdir);
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
           pthread_exit(NULL);
        } else if (ch == 'o') {
            dstfd = open(destname, O_RDWR|O_TRUNC, 0644);
        } else if (ch == 'a') {
            dstfd = open(destname, O_RDWR|O_APPEND, 0644);
        } else {
            printf("input error!\n");
            close(srcfd);
            pthread_exit(NULL);
        }
    } else {
        dstfd = open(destname, O_RDWR|O_CREAT, 0644);
    }
    if (dstfd < 0) {
        perror("open destfile wrong!");
        pthread_exit(NULL);
    }

    while(1) {
        memset(buffer, 0, 128);
        len = read(srcfd, buffer, 127);
        write(dstfd, buffer, 127);
        if (len < 127) {
            break;
        }
    }
    close(srcfd);
    close(dstfd);
    pthread_exit(NULL);
}
