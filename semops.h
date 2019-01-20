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


#define POP_SIZE 2
#define KEY 1493
#define KEYBIN 1494
#define shmkey 123
#define msgkey 2001
#define msgkeyReply 2002
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    #if defined(__linux__)
    struct seminfo* __buf;
    #endif
};

struct MyInvites{
    long mtype;
    int from;
    int vote;
    int group;
}myInvite;

struct MyReplys{
    long mtype;
    int from;
    bool reply;
}myReply;


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
    bool group;
}student;

struct sembuf* generateReduceOp();


struct sembuf* generateWaitOp();


struct sembuf* generateIncreaseOp();