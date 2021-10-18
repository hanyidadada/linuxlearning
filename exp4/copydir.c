#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

void copydir(DIR *dir, const char workdir[]);

int main(int argc, char *argv[]){
    int i, n;
    DIR *dir;
    char workdir[128];
    if (argc > 2) {
        perror("too many argv!");
        exit(-1);
    }
    if (argc == 2) {
        strcpy(workdir, argv[1]);
    } else {
        if(getcwd(workdir, 128) == NULL) {
            perror("get current work dir error");
            exit(-1);
        }
    }
    printf("\t%s\n", workdir);
    dir = opendir(workdir);
    if (dir == NULL) {
        perror("open dirtent error! check if exit:");
        exit(-1);
    }
    
    copydir(dir, workdir);
    closedir(dir);
    printf("copy end\n");
    return 0;
}

void copydir(DIR *dir, const char workdir[])
{
    struct dirent *currentdp = NULL;
    char filepath[128];
    struct stat fstat;
    pid_t pid;
    
    while((currentdp = readdir(dir))!= NULL) {
        memset(filepath, 0, 128);
        strcpy(filepath, workdir);
        strcat(filepath, "/");
        strcat(filepath, currentdp->d_name);
        lstat(filepath, &fstat);
        if (!strcmp(currentdp->d_name, ".") || !strcmp(currentdp->d_name, "..")) {
            continue;
        }
                
        if(!S_ISDIR(fstat.st_mode)){
            pid = fork();
            if (pid < 0) {
                perror("fork error");
                exit(-1);
            } else if (pid == 0) {
                execl("/root/linuxexp/exp2/filecopy", ".filecopy", filepath, "/home/ywj/", NULL);
                exit(0);
            } else {
                waitpid(pid, NULL, 0);
                printf("copy %s end\n", filepath);
            }
        } else {
            pid = fork();
            if (pid < 0) {
                perror("fork error");
                exit(-1);
            } else if (pid == 0) {
                execlp("./copydir", "./copydir", filepath, NULL);
                exit(0);
            } else {
                waitpid(pid, NULL, 0);
            }
        }
    }
}

