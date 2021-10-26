#ifndef __HEADER_H
#define __HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

#define NUM 5

#define SHM_KEY 0x1234
#define SEM_KEY 0x8abbc
#define SEM_EMPTY 0
#define SEM_FULL  1
#define SEM_MUTEX 2

#define CONSUMER "consumer"
#define PRODUCER "producer"

struct BufferPool {  
    char Buffer[NUM][100];//5个缓冲区
    int Index[NUM];   //缓冲区状态：为0表示对应的缓冲区未被生产者使用，可分配但不可消费；为1表示对应的缓冲区以被生产者使用，不可分配但可消费
};

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};
void init_allsem(int *semid);
int init_sem(int sem_id, int semnum, int init_value);
int semaphore_p (int sem_id,short sem_no);
int semaphore_v (int sem_id,short sem_no);
int del_sem(int sem_id, int semnum);
int sem_getval(int sem_id, int semnum);
#endif
