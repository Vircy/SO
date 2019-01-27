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
#include<signal.h>


#define POP_SIZE 20
#define KEY 1493
#define KEYBIN 1494
#define KEYTWO 1495
#define shmkey 123
#define shmkeyB 124
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
    int willsize;
}myInvite;

struct MyReplys{
    long mtype;
    int from;
    bool reply;
    int vote;
    int willSize;
}myReply;


struct Groups{
    int group_leader_id;
    int size; 
    int leader_willsize;
    bool closed;
    int max_vote;
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