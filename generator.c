#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "semops.h" // my header


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

int semBinInit(){
    int semIdB;
    
    // Check if semkey already exists, if so delete it
    semIdB = semget(KEYBIN, 1, 0666);
    if (semIdB != -1) {
        semctl(semIdB, 0, IPC_RMID);
    }

    // Create new semaphore
    semIdB = semget(KEYBIN, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if(semIdB == -1) {
        printf("error on semaphore creation");
        exit(-1);
    }

    // Initialise semaphore
    union semun argsB;
    argsB.val = 0; //+debug
    semctl(semIdB, 0, SETVAL, argsB.val);
    return semIdB;
}




int main(int argc, char** argv){

    //printf("Startig ...");
    int sem = seminit();
    int semBin = semBinInit();
    int size=0;
    int shmId;
    int *shmp;
    int t=0;
    struct shmid_ds *buf; 

    //Creae a shared memory
    shmId = shmget(shmkey ,sizeof(int)*(POP_SIZE) , IPC_CREAT | IPC_EXCL | 0666);
    printf("shmID di creazione = %d", shmId);

    for(int i = 0; i < POP_SIZE; i++) { 
       printf("\nStarting student %d", i);
        int fork_res = fork();
        if(fork_res == 0) {
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
    for(int i=0;i<POP_SIZE;i++){ //waiting sons 

     wait(NULL);
    }  

    
    shmp = ( int*)shmat(shmId, NULL, 0); 
    shmctl(shmId,IPC_SET, buf);
     printf("\n wait terminata, il valore di shmp in generator è %p", shmp);
    while(t < POP_SIZE){
        printf("\nSHM figlio con pid : %d", shmp[t]);
        t++;
    }
    shmctl(shmId,IPC_RMID, NULL);
}