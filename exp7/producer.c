#include "header.h"

int main(int argc, char *argv[]){  
    int i, ret;
    char filename[128];
    void *shm = NULL; 
    struct BufferPool *shared;
    int shmid, semid, fd;
    if (argc < 2){
        perror("argv num error");
        exit(EXIT_FAILURE);
    }
    strcpy(filename, PRODUCER);
    strcat(filename, argv[1]);
    fd = open(filename, O_RDONLY);

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
        sleep(rand() % NUM);
        semaphore_p(semid, SEM_EMPTY);
        semaphore_p(semid, SEM_MUTEX);

        for (i = 0; i < NUM; i++) {
            if (shared->Index[i] == 0){
                break;
            }
        }
        if (i >= NUM) {
            semaphore_v(semid, SEM_MUTEX);
            semaphore_v(semid, SEM_EMPTY);
            continue;
        }
        memset(shared->Buffer[i], 0, 100);
        if(read(fd, shared->Buffer[i], 99) == 0){
            printf("\tProducer %s: ALL FILE READ OVER\n", argv[1]);
            semaphore_v(semid, SEM_MUTEX);
            semaphore_v(semid, SEM_FULL);
            break;
        }
        shared->Buffer[i][strlen(shared->Buffer[i])] = '\0';
        printf("Producer %s:Read from text: %s\n", argv[1], shared->Buffer[i]);
        shared->Index[i] = 1;
        semaphore_v(semid, SEM_MUTEX);
        semaphore_v(semid, SEM_FULL);  
    }
    if(shmdt(shm) == -1) {
        exit(EXIT_FAILURE);
    }  
    if(shmctl(shmid, IPC_RMID, 0) == -1){
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

