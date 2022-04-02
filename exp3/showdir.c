#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <dirent.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

void showdir(DIR *dir, char flag[], const char workdir[]);
void showfiletype(struct stat *fstat);
void showfilemode(struct stat *fstat);
void showdpinfo(struct dirent *currentdp, const char workdir[], int sflag);

int main(int argc, char *argv[]){
    int i, n;
    DIR *dir;
    char workdir[128];
    char flag[128];

    for (i = 1; i < argc; i++) {
        strcat(flag, argv[i]+1);
    }

    if(getcwd(workdir, 128) == NULL) {
        perror("get current work dir error");
        exit(-1);
    }
    dir = opendir(workdir);

    showdir(dir, flag, workdir);
    closedir(dir);
    return 0;
}

void showdir(DIR *dir, char flag[], const char workdir[])
{
    struct dirent *currentdp = NULL;
    int aflag = 0;
    int Aflag = 0;
    int sflag = 0;
    if (strchr(flag, 'a') != NULL) {
        aflag = 1;
    }
    if (strchr(flag, 'A') != NULL) {
        Aflag = 1;
    }
    if (strchr(flag, 's') != NULL) {
        sflag = 1;
    }
    

    if (strchr(flag, 'l') != NULL) {
        while((currentdp = readdir(dir))!= NULL) {
            if (currentdp->d_name[0] == '.' && !aflag && !Aflag) {
                continue;
            }
            if (currentdp->d_name[0] == '.' && Aflag) {
                if (!strcmp(currentdp->d_name, ".") || !strcmp(currentdp->d_name, "..")) {
                    continue;
                }
            }
            showdpinfo(currentdp, workdir, sflag);
        }
    } else {
        while((currentdp = readdir(dir))!= NULL) {
            if (currentdp->d_name[0] == '.' && !aflag && !Aflag) {
                continue;
            }
            if (currentdp->d_name[0] == '.' && Aflag) {
                if (!strcmp(currentdp->d_name, ".") || !strcmp(currentdp->d_name, "..")) {
                    continue;
                }
            }
            printf("%s ", currentdp->d_name);
        }
        printf("\n");
    }
   
}

void showfiletype(struct stat *fstat)
{
    if (S_ISREG(fstat->st_mode)) {
        printf("-");
    } else if (S_ISDIR(fstat->st_mode)) {
        printf("d");
    } else if (S_ISCHR(fstat->st_mode)) {
        printf("c");
    } else if (S_ISBLK(fstat->st_mode)) {
        printf("b");
    } else if (S_ISFIFO(fstat->st_mode)) {
        printf("p");
    } else if (S_ISSOCK(fstat->st_mode)) {
        printf("s");
    } else if (S_ISLNK(fstat->st_mode)) {
        printf("l");
    }

    return ;
}

void showfilemode(struct stat *fstat)
{

    (fstat->st_mode&S_IRWXU)&S_IRUSR ? printf("r") : printf("-");
    (fstat->st_mode&S_IRWXU)&S_IWUSR ? printf("w") : printf("-");
    (fstat->st_mode&S_IRWXU)&S_IXUSR ? printf("x") : printf("-");
    (fstat->st_mode&S_IRWXG)&S_IRGRP ? printf("r") : printf("-");
    (fstat->st_mode&S_IRWXG)&S_IWGRP ? printf("w") : printf("-");
    (fstat->st_mode&S_IRWXG)&S_IXGRP ? printf("x") : printf("-");
    (fstat->st_mode&S_IRWXO)&S_IROTH ? printf("r") : printf("-");
    (fstat->st_mode&S_IRWXO)&S_IWOTH ? printf("w") : printf("-");
    (fstat->st_mode&S_IRWXO)&S_IXOTH ? printf("x") : printf("-");
    return ;
}

void showdpinfo(struct dirent *currentdp, const char workdir[], int sflag) {
    char filepath[128];
    char buffer[100];
    mode_t mode;
    struct stat filestat;
    /* get file path */
    strcpy(filepath, workdir);
    strcat(filepath, "/");
    strcat(filepath, currentdp->d_name);
    lstat(filepath, &filestat);
    
    mode = filestat.st_mode;
    if (S_ISLNK(filestat.st_mode) && sflag) {
        stat(filepath, &filestat); 
    }    
    showfiletype(&filestat);
    showfilemode(&filestat);
    
    printf("%2d %s %s %5d ", filestat.st_nlink, getpwuid(filestat.st_uid)->pw_name, getgrgid(filestat.st_gid)->gr_name, filestat.st_size);

    strftime(buffer, 100, "%b %d %H:%M", localtime(&filestat.st_mtime));
    printf("%s %s", buffer, currentdp->d_name);
    if (S_ISLNK(mode)) {
        memset(&buffer, 0, 100);
        printf(" -> ");
        readlink(filepath, buffer, 99);
        printf("%s", buffer);
    }
    printf("\n");   
}
