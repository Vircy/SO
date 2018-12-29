#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY (1491)
#define POP_SIZE 5

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    #if defined(__linux__)
    struct seminfo* __buf;
    #endif
};

int seminit(){
    int semId;

    semId = semget(KEY, 1, 0644 | IPC_CREAT);
    if(semId == -1 ){
        printf("error on semaphore creation");
        exit(-1);        
    }

    union semun args;
    args.val = POP_SIZE;

    semctl(semId, 0, SETVAL, &args);
 
    return semId;
}

struct sembuf* generateWaitOp(){
    struct sembuf* semops = malloc(sizeof(struct sembuf)*2);

    struct sembuf* decrementOp = semops;
    decrementOp->sem_num = 0;
    decrementOp->sem_op = -1;
    decrementOp->sem_flg = IPC_NOWAIT;

    struct sembuf* waitOp = semops + 1;
    waitOp->sem_num = 0;
    waitOp->sem_op = 0;
    waitOp->sem_flg = IPC_NOWAIT;
    
    return semops;
}


int main(int argc, char** argv){
    int sem = seminit();
    struct sembuf* waitOp = generateWaitOp();

    for(int i=0;i<POP_SIZE;i++){ // loop will run n times 
        
        ///printf("\n POP_SIZE : %d" , i);
        if(fork() == 0){
            semop(sem, waitOp, 2);
            //printf("\nwait\n");
            execve ("STUDENT", argv, NULL);
         } 
    } 
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times (n=5) 
     wait(NULL);
    }
      
} 