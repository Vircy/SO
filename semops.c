#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <sys/msg.h>


//#define POP_SIZE 5
//#define KEY 1493
///#define shmkey IPC_PRIVATE

struct MyInvites{
    long mtype;
    int vote;
    int group;
}myInvite;

struct MyResposnes{
    long mtype;
    bool responese;
}myResponse;


struct Groups{
    int size ;
    int leader;
    int mSize;
    bool full;
}group;

struct Students {
    int turn;
    int id;
    int vote;
    int groupSize;
    bool assigned;
    bool closedGrouop;
}student;


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
struct sembuf* generateIncreaseOp(){
    struct sembuf* semIncrease = malloc(sizeof(struct sembuf)*1);
    
    struct sembuf* increaseOp = &semIncrease[0];
    increaseOp->sem_num = 0;
    increaseOp->sem_op = 1;
    increaseOp->sem_flg = 0;

    return semIncrease;
}
