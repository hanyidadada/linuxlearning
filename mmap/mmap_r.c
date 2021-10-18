#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
struct STU{
    int id;
    char name[10];
    char sex;
};

int main(int argc, char *argv[]){
    struct STU *mm;
    int fd;
    if(argc < 2) {
        perror("./a.out filename_shared");
        exit(-1);
    }
    
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open file error:");
        exit(-1);
    }
    mm = mmap(NULL, sizeof(struct STU), PROT_READ, MAP_SHARED, fd, 0);
    if (mm == MAP_FAILED) {
        perror("mmap error:");
        exit(-1);
    }
    close(fd);

    while(1){
        if(mm->id < 0) {
            printf("Now, reader exit!\n");
            break;
        }
        if (mm->id == 0) {
            sleep(1);
            continue;
        }
        printf("student id: %d, name:%s, sex:%c\n", mm->id, mm->name, mm->sex);
        sleep(1);
    }
    munmap(mm, sizeof(struct STU));
    return 0;
}
