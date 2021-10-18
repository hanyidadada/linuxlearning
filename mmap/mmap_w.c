#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

struct STU{
    int id;
    char name[10];
    char sex;
};

int main(int argc, char *argv[]){
    struct STU *mm;
    struct STU std;
    int fd;
    if(argc < 2) {
        perror("./a.out filename_shared");
        exit(-1);
    }
    unlink(argv[1]);
    fd = open(argv[1], O_RDWR|O_CREAT, 0644);
    if (fd < 0) {
        perror("open file error:");
        exit(-1);
    }
    ftruncate(fd, sizeof(std));

    mm = mmap(NULL, sizeof(struct STU), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mm == MAP_FAILED) {
        perror("mmap error:");
        exit(-1);
    }
    close(fd);

    while(1){
        memset(mm, 0, sizeof(std));
        printf("Enter Student info(id < 0 to exit):\n");
        printf("id:");
        scanf("%d", &(std.id));
        printf("name:");
        scanf("%s", std.name);
        printf("sex:");
        scanf(" %c", &(std.sex));
        memcpy(mm, &std, sizeof(std));
        if(mm->id < 0) {
            printf("Now, writer exit!\n");
            break;
        }
        sleep(1);
    }
    munmap(mm, sizeof(struct STU));
    unlink(argv[1]);
    return 0;
}
