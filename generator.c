#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY (1492)
#define POP_SIZE 5

int seminit(){
    int sem;
    struct sembuf* semops = malloc(sizeof(struct sembuf)*1);
    semops->sem_num = 0;
    semops->sem_op = POP_SIZE;
    semops->sem_flg = IPC_NOWAIT;

    sem = semget(KEY, 1, 0644 | IPC_EXCL | IPC_CREAT);
    semop(sem, semops, 1);

    return sem;
}

struct sembuf* generateWaitOp(){
    struct sembuf* semops = malloc(sizeof(struct sembuf)*2);

    struct sembuf* decrementOp = semops;
    decrementOp->sem_num = -1;
    decrementOp->sem_op = POP_SIZE;
    decrementOp->sem_flg = IPC_NOWAIT;

    struct sembuf* waitOp = semops + 1;
    waitOp->sem_num = 0;
    waitOp->sem_op = POP_SIZE;
    waitOp->sem_flg = IPC_NOWAIT;
    
    return semops;
}


int main(int argc, char** argv){
    int sem = seminit();
    struct sembuf* waitOp = generateWaitOp();
    
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times 
        
        printf("\n POP_SIZE : %d" , i);
        if(fork() == 0){
            semop(sem, waitOp, 2);
            execve ("STUDENT", argv, NULL);
         } 
    } 
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times (n=5) 
     wait(NULL);
    }
      
} 