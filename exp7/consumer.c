#include "header.h"

int main(int argc, char *argv[]){  
    int i, ret;
    int flag = 0;
    char ch;
    char filename[128];
    void *shm = NULL; 
    struct BufferPool *shared;
    int shmid, semid;
    FILE *file;
    if (argc < 2){
        perror("argv num error");
        exit(EXIT_FAILURE);
    }
    strcpy(filename, CONSUMER);
    strcat(filename, argv[1]);
    file = fopen(filename, "wr");

    shmid = shmget((key_t)SHM_KEY, sizeof(struct BufferPool), 0666|IPC_CREAT);  
    if(shmid == -1){  
       exit(EXIT_FAILURE);
    }  
    shm = shmat(shmid, 0, 0);  
    if(shm == (void*)-1) {  
       exit(EXIT_FAILURE);
    }  
    shared = (struct BufferPool*)shm;

    init_allsem(&semid);

    while(1){ 
        sleep(rand()%NUM);
        semaphore_p(semid, SEM_FULL);
        semaphore_p(semid, SEM_MUTEX);

        for (i = 0; i < NUM; i++) {
            if (shared->Index[i]){
                break;
            }
        }
        if (i >= NUM) {
            flag++;
            semaphore_v(semid, SEM_MUTEX);
            semaphore_v(semid, SEM_FULL);
            if (flag > 10) {
                printf("\n Consumer %s:should I exit(y or n):", argv[1]);
                scanf(" %c", &ch);
                if (ch == 'y') {
                    break;
                } else {
                    flag = 0;
                }
            }
            continue;
        }
        flag = 0;
        printf("Consumer %s: %s\n", argv[1],shared->Buffer[i]);
        shared->Index[i] = 0;
        fprintf(file, "%s\n", shared->Buffer[i]);
        semaphore_v(semid, SEM_MUTEX);
        semaphore_v(semid, SEM_EMPTY);
    }
    // del_sem(semid, SEM_EMPTY);
    
    if(shmdt(shm) == -1) {
        exit(EXIT_FAILURE);
    }  
    if(shmctl(shmid, IPC_RMID, 0) == -1){
        exit(EXIT_FAILURE);
    }
    fclose(file);
    exit(EXIT_SUCCESS);
}

