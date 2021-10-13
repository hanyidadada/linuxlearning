#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_ARGC 999
char filename[30];

void analywords(char* argv[][20], int argc[], int *num);
void showcmd(char* argv[][20], int argc[], int num);
void redirect_in(char *filename);
void redirect_out(char *filename);
void executecmd(char* argv[][20], int argc[], int num);
void pipexec(char* argv[][20], int argc[], int i);
int main(void)
{
    pid_t pid;
    int num = 0;
    char* argv[20][20];
    int argc[20];
    while(1) {
        memset(argc, 0, sizeof(argc));
        memset(argv, 0, sizeof(argv));
        num = 0;
        printf("Please enter cmd:");
        analywords(argv, argc, &num); // 解析命令
        if (strcmp("exit", argv[0][0]) == 0) {
            break;
        }
        
        executecmd(argv, argc, num);
        
        // showcmd(argv, argc, num);  // 打印所有命令信息
               
    }
    printf("Bye!\n");
    exit(0);
}

void showcmd(char* argv[][20], int argc[], int num)
{
    int i, j;
    for (i = 0; i < num; i++) {
        printf("number %d cmd: \n", i+1);
        printf("nums of argv: %d \t", argc[i]);
        for (j = 0; j < argc[i]; j++) {
            printf("%s ", argv[i][j]);
            if (argc[i] > MAX_ARGC) {
                break;
            }   
        }
        printf("\n");            
    }
    printf("\n");
}

void analywords(char* argv[][20], int argc[], int *num)
{
    char ch;
    int i=0, j = 0;
    int blankflag = 0;
    char argvAr[20][20][128];
    while (1) {
        if (*num > 20) {
            printf("too many command\n");
            exit(-1);
        }
    A: ch = getchar();
        switch (ch) {
            case '>':
                if (blankflag) {
                    argc[*num] = i + 1;
                    blankflag = 0;
                } else {
                    argc[*num] = i;
                }
                (*num)++;
                i = j = 0;
                argvAr[*num][i][j] = ch;
                argc[*num] = MAX_ARGC+1;
                (*num)++;
                continue;
                break;
            case '<':
                if (blankflag) {
                    argc[*num] = i + 1;
                    blankflag = 0;
                } else {
                    argc[*num] = i;
                }
                (*num)++;
                i = j = 0;
                argvAr[*num][i][j] = ch;
                argc[*num] = MAX_ARGC+2;
                (*num)++;
                continue;
                break;
            case '|':
                 if (blankflag) {
                    argc[*num] = i + 1;
                    blankflag = 0;
                } else {
                    argc[*num] = i;
                }
                (*num)++;
                i = j = 0;
                argvAr[*num][i][j] = ch;
                argc[*num] = MAX_ARGC+3;
                (*num)++;
                continue;
                break;
            case '\n':
                argc[*num] = i + 1;               
                (*num)++;
                goto B;
                break; 
            case ' ':
                blankflag = 1;
                goto A;
                break;
            default:
                break;
        }
        if (blankflag) {
            if (j != 0) {
                i++;
                j=0;
                argc[*num] += 1;
            }
            blankflag = 0;
        }
        argvAr[*num][i][j] = ch;
        j++;
    }
 B: for(i = 0; i < *num; i++) {
        for (j = 0; j < argc[i]; j++) {
            argv[i][j] = (char*)malloc(sizeof(char)*(strlen(argvAr[i][j])+1));
            strcpy(argv[i][j],argvAr[i][j]);
            if (argc[i] > MAX_ARGC) {
                j++;
                break;
            }  
        }
        argv[i][j] = NULL;
    }
}

void executecmd(char* argv[][20], int argc[], int num)
{
    pid_t pid;
    int i = 0;
    pid = fork();
    if (pid < 0) {
        perror("fork error:");
        exit(-1);
    } else if (pid == 0) {
        for (i = 0; i < num; i++) {
            if (argc[i] == MAX_ARGC + 1) {
                // 输出重定向
                redirect_out(argv[i+1][0]);
                free(argv[i][0]);
                free(argv[i+1][0]);
            } else if (argc[i] == MAX_ARGC + 2) {
                // 输入重定向
                redirect_in(argv[i+1][0]);
                free(argv[i][0]);
                free(argv[i+1][0]);
            }
        }
        pipexec(argv, argc, 0);
        
        exit(0);
    } else {
        wait(NULL);
        return ;
    }
}


//输入重定向
void redirect_in(char *filename)
{
    int fd;
   
    fd=open(filename,O_RDONLY);
    if (fd==-1)
    {
        printf("file open failed\n");
        return;
    }
    dup2(fd,0);
    close(fd);

}

//输出重定向
void redirect_out(char *filename)
{
    int fd;

    fd=open(filename,O_CREAT|O_RDWR,0666); //0666为权限
    if (fd==-1)
    {
        printf("file open failed\n");
        return;
    }
   
    dup2(fd,1);
    close(fd);
}

void pipexec(char* argv[][20], int argc[], int i)
{
    if (argc[i+1] != MAX_ARGC+3) {     
        execvp(argv[i][0], argv[i]);
    }
    
    int fd[2];
    pipe(fd);//创建管道，0读，1写
    if (fork() == 0)
    {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execvp(argv[i][0], argv[i]);
    }
    dup2(fd[0], 0);
    close(fd[0]);
    close(fd[1]);
    pipexec(argv, argc, i+2);
}

