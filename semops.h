#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define POP_SIZE 5
#define KEY 1493
#define shmkey 123 //IPC_PRIVATE

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    #if defined(__linux__)
    struct seminfo* __buf;
    #endif
};

struct sembuf* generateReduceOp();


struct sembuf* generateWaitOp();
