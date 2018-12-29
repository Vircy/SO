#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY (1492)

int main(int argc, char** argv){
    unsigned int POP_SIZE = 5; 
    int sem;
    struct sembuf* semops = malloc(sizeof(struct sembuf)*1);
    semops->sem_num = 0;
    semops->sem_op = POP_SIZE;
    semops->sem_flg = IPC_NOWAIT;

    sem = semget(KEY, 1, 0644 | IPC_EXCL | IPC_CREAT);
    semop(sem, semops, 1);
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times 

        printf("\n POP_SIZE : %d" , i);
        if(fork() == 0){  
            execve ("STUDENT", argv, NULL);
         } 
    } 
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times (n=5) 
     wait(NULL);
    }
      
} 