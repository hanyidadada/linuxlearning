#include "header.h"

void init_allsem(int *semid){
    int ret;
    *semid = semget(SEM_KEY, 3, 0);
    if (*semid < 0){
        *semid = semget(SEM_KEY, 3, IPC_CREAT|0666); // 创建两个信号量1：empty 2: full
        ret = init_sem(*semid, SEM_EMPTY, NUM);
        if(ret < 0) {
            exit(EXIT_FAILURE);
        }
        ret = init_sem(*semid, SEM_FULL, 0);
        if(ret < 0) {
            exit(EXIT_FAILURE);
        }
        ret = init_sem(*semid, SEM_MUTEX, 1);
        if(ret < 0) {
            exit(EXIT_FAILURE);
        }
    }
    
}
int init_sem(int sem_id, int semnum, int init_value){
    
    union semun sem_union;
    sem_union.val = init_value;
    if(semctl(sem_id, semnum, SETVAL, sem_union)==-1)
	{
		perror("Initialize semaphore");
		return -1;
	}
    return 0;
}

int semaphore_p(int sem_id,short sem_no)
{
    struct sembuf sem_b;
    sem_b.sem_num = sem_no;//信号量集中信号量编号
    sem_b.sem_op = -1; // P操作，每次分配1个资源
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) 
    {
        perror("semaphore_p failed");
        return -1;
    }
    return 0;
}

int semaphore_v (int sem_id,short sem_no)
{
    struct sembuf sem_b;
    sem_b.sem_num = sem_no;//信号量集中信号量编号
    sem_b.sem_op = 1; //V操作，每次释放1个资源
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) 
    {
    perror("semaphore_v failed");
    return 0;
    }
    return 1;
}

int del_sem(int sem_id, int semnum)
{
    union semun sem_union;
	if(semctl(sem_id, semnum, IPC_RMID, sem_union)==-1)
	{
		perror("Delete semaphore");
		return -1;
	}
    return 0;
}

int sem_getval(int sem_id, int semnum)
{
    union semun sem_union;
	return semctl(sem_id, semnum, GETVAL, sem_union);
}
