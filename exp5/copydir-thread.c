#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

extern void* copyfile_thread(void *arg);

void *copydir_thread(void * arg);

struct thread_arg{
    char workdir[128];
    char destdir[128];
};

int main(int argc, char *argv[]){
    int ret;
    pthread_t tid;
    DIR *dir;
    struct thread_arg arg;

    if (argc > 3) {
        perror("too many argv!");
        exit(-1);
    }
    if (argc == 2) {
        if(getcwd(arg.workdir, 128) == NULL) {
            perror("get current work dir error");
            exit(-1);
        }
        strcpy(arg.destdir, argv[1]);
    } else if (argc == 3) {
        strcpy(arg.destdir, argv[2]);
        strcpy(arg.workdir, argv[1]);
        if (arg.workdir[strlen(arg.workdir) - 1] == '/') {
            arg.workdir[strlen(arg.workdir) - 1] = '\0';
        }
        
    } else {
        if(getcwd(arg.workdir, 128) == NULL) {
            perror("get current work dir error");
            exit(-1);
        }
        strcpy(arg.destdir, "/home/ywj/");
    }
    
    ret = pthread_create(&tid, NULL, copydir_thread, (void *)&arg);
    if (ret != 0) {
        perror("create thread error\n");
        exit(-1);
    }
    pthread_join(tid, NULL);
    return 0;
}

void *copydir_thread(void *arg)
{
    struct dirent *currentdp = NULL;
    struct thread_arg outarg;
    struct thread_arg *inarg = (struct thread_arg *) arg;
    pthread_t tid;
    int ret;
    char filepath[128];
    struct stat fstat;
    DIR *dir, *ddir;

    dir = opendir(inarg->workdir);
    if (dir == NULL) {
        perror("open dirtent error! check if exit:");
        exit(-1);
    }
    while((currentdp = readdir(dir))!= NULL) {
        memset(&outarg, 0, sizeof(struct thread_arg));
        memset(filepath, 0, 128);
        memset(&fstat, 0, sizeof(struct stat));
        strcpy(filepath, inarg->workdir);
        strcat(filepath, "/");
        strcat(filepath, currentdp->d_name);
        lstat(filepath, &fstat);
        if (!strcmp(currentdp->d_name, ".") || !strcmp(currentdp->d_name, "..")) {
            continue;
        }
                
        if(!S_ISDIR(fstat.st_mode)){
            strcpy(outarg.workdir, filepath);
            strcpy(outarg.destdir, inarg->destdir);
            ret = pthread_create(&tid, NULL, copyfile_thread, (void *)&outarg);
            if (ret != 0) {
                perror("create thread error\n");
                exit(-1);
            } 
        } else {
            strcpy(outarg.workdir, filepath);
            strcpy(outarg.destdir, inarg->destdir);
            if (outarg.destdir[strlen(outarg.destdir)-1] == '/') {
                strcat(outarg.destdir, currentdp->d_name);
            } else {
                strcat(outarg.destdir, "/");
                strcat(outarg.destdir, currentdp->d_name);
            }
            
            ddir = opendir(outarg.destdir);
            if (ddir == NULL) {
                mkdir(outarg.destdir, 0777);
            } else {
                closedir(ddir);
            }
            ret = pthread_create(&tid, NULL, copydir_thread, (void *)&outarg);
            if (ret != 0) {
                perror("create thread error\n");
            }
        }
        pthread_join(tid, NULL);
        printf("\tcopy %s to %s end\n", outarg.workdir, outarg.destdir);
    }
    closedir(dir);
}

