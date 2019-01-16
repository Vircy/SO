#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdbool.h>
#define POP_SIZE 5
#define KEY 1493
#define KEYBIN 1494
#define shmkey 123

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    #if defined(__linux__)
    struct seminfo* __buf;
    #endif
};

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

struct sembuf* generateReduceOp();


struct sembuf* generateWaitOp();


struct sembuf* generateIncreaseOp();