#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "semops.h" // my header
#include <stdbool.h>

#define DEBUG 0

void print_groups(struct Groups *shmpB){
    int i=0;
    while(shmpB[i].max_vote != 0){
        printf("\ngruppo numero %d , il voto del gruppo è %d ,dimensione del gruppo = %d , il gruppo di %d è chiuso ? %d", i , shmpB[i].max_vote,shmpB[i].size , shmpB[i].group_leader_id ,shmpB[i].closed );
        i++;
    }
}

void stop_child(struct Students * shmp){
    int i=0;
    sleep(8);
    while(shmp[i].id != 0){
        kill(shmp[i].id, SIGINT);
        i++;
    }
}




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

int semBinInit(int x){
    int semIdB;
    
    // Check if semkey already exists, if so delete it
    semIdB = semget(x, 1, 0666);
    if (semIdB != -1) {
        semctl(semIdB, 0, IPC_RMID);
    }

    // Create new semaphore
    semIdB = semget(x, 1, 0666 | IPC_CREAT | IPC_EXCL);
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
    int sim_time = 10;
    int sem = seminit();
    int semBin = semBinInit(KEYBIN);
    int semBinTwo = semBinInit(KEYTWO);
    int size=0;
    int shmId;
    int shmIdB;
    struct Students *shmp;
    struct Groups *shmpB;
    int t=0;
    struct shmid_ds *buf; 
    int clean;
    int msgqI;
    int msgqR;
    int cecker;
    //Creae a shared memory
    if((shmId = shmget(shmkey ,0 , 0)) != -1){
        clean = shmctl(shmId,IPC_RMID, NULL);
        printf("pulisco una vecchia SHM, funzione = %d", clean);
    }
    if((shmIdB = shmget(shmkeyB ,0 , 0)) != -1){
        clean = shmctl(shmIdB,IPC_RMID, NULL);
        printf("pulisco una vecchia SHM, funzione = %d", clean);
    }
    
    if((msgqI = msgget(msgkey , 0666 ))!=-1){
        printf("errore nella creazione della MSGQ");
        msgctl(msgqI , IPC_RMID, NULL);////////////////////////////////////DA VERIFICARE L'ULIMO PARAMETRO
    }
    if((msgqR = msgget(msgkeyReply , 0666 ))!=-1){
        printf("errore nella creazione della MSGQ");
        msgctl(msgqR , IPC_RMID, NULL );////////////////////////////////////DA VERIFICARE L'ULIMO PARAMETRO
    }
    msgqI = msgget (msgkey , IPC_CREAT | 0666);
    printf("\n msgI = %d", msgqI);
    msgqR = msgget (msgkeyReply, IPC_CREAT | 0666);
    printf("\n msgR = %d", msgqR);
    shmId = shmget(shmkey ,sizeof(struct Students)*(POP_SIZE) , IPC_CREAT | IPC_EXCL | 0666);
    if(shmId == -1){
        printf("errore");
        exit(-1);
    }
    shmIdB = shmget(shmkeyB ,sizeof(struct Groups)*(POP_SIZE) , IPC_CREAT | IPC_EXCL | 0666);
    if(shmIdB == -1){
        printf("errore");
        exit(-1);
    }

    for(int i = 0; i < POP_SIZE; i++) { 
       printf("\nStarting student %d", i);
        int fork_res = fork();
        if(fork_res == 0) {
            int err = execve("STUDENT", argv, NULL);
            if (err == -1) {
                printf("\nError executing student process");
                exit(1);
            }
        
        }else if (fork_res == -1) {
            printf("\nError forking parent process");
            exit(1);
        }
        printf("\n started student %d with pid = %d\n", i , fork_res);
    }

    struct sembuf* waitOp = generateWaitOp();
    semop(sem, waitOp, 1); 
    shmp = ( struct Students*)shmat(shmId, NULL, 0);
    shmpB = (struct Groups*)shmat(shmIdB, NULL , 0);
    stop_child(shmp);
   // sleep(sim_time);
    //kill();

    for(int i=0;i<POP_SIZE;i++){ //waiting sons 
     wait(NULL);
    }  

    shmctl(shmId,IPC_SET, buf);
    while(t < POP_SIZE){
        printf("\nSHM figlio con pid : %d , voto %d , stato di gruppo = %d e dimensione desiderata %d", shmp[t].id , shmp[t].vote, shmp[t].group , shmp[t].groupSize);
        t++;
    }
    print_groups(shmpB);
    printf ("\n   in vero in print true = %d" , true);
    if(shmctl(shmId,IPC_RMID, NULL)==-1){
        printf("\n error on shm remove");
        exit(-1);
    }
    msgctl(msgqI , IPC_RMID, NULL);
    msgctl(msgqR , IPC_RMID, NULL);
    shmctl(shmId,IPC_RMID, NULL);
    shmctl(shmIdB,IPC_RMID, NULL);
}