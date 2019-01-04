#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "semops.h" // my header

#define KEY 1493
#define POP_SIZE 5

#define DEBUG 0

int seminit(){
    int semId;

    // Check if semkey already exists, if so delete it
    semId = semget(KEY, 1, 0666);
    if (semId != -1) {
        semctl(semId, 0, IPC_RMID);
    }

    // Create new semaphore
    semId = semget(KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if(semId == -1) {
        printf("error on semaphore creation");
        exit(-1);
    }

    // Initialise semaphore
    union semun args;
    args.val = POP_SIZE; //+debug
    semctl(semId, 0, SETVAL, args.val);
    return semId;
}



int main(int argc, char** argv){

    //printf("Startig ...");
    int sem = seminit();
    int size=0;
    printf("\nSem inited");

    for(int i = 0; i < POP_SIZE; i++) { // loop will run n times 
       printf("\nStarting student %d", i);
        int fork_res = fork();
        if(fork_res == 0) {
            //printf("\nForked");
            //printf("\nTest");
           // printf("\nTest");
            int err = execve("STUDENT", argv, NULL);
            if (err == -1) {
                printf("\nError executing student process");
                exit(1);
            }
        }
        else if (fork_res == -1) {
            printf("\nError forking parent process");
            exit(1);
        }
    }
    struct sembuf* waitOp = generateWaitOp();
    semop(sem, waitOp, 1);
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times (n=5) 
     wait(NULL);
    }     
}