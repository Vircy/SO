#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 1493
#define shmkey IPC_PRIVATE

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    #if defined(__linux__)
    struct seminfo* __buf;
    #endif
};

struct sembuf* generateReduceOp(){
    struct sembuf* semReduce = malloc(sizeof(struct sembuf)*1);
    
    struct sembuf* decrementOp = &semReduce[0];
    decrementOp->sem_num = 0;
    decrementOp->sem_op = -1;
    decrementOp->sem_flg = 0;

    return semReduce;
}
struct sembuf* generateWaitOp(){
    struct sembuf* semWait = malloc(sizeof(struct sembuf)*1);
    struct sembuf* waitOp = &semWait[0];
    waitOp->sem_num = 0;
    waitOp->sem_op = 0;
    waitOp->sem_flg = 0;
    
    return semWait;
}