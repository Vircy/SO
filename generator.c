
#include "semops.h" // my header


void print_groups(struct Groups *shmpB , struct Students *shmp){
    int i=0;
    int students_n=0;
    int vote_counter=0;
    int medium_vote=0;
    while(shmpB[i].max_vote != 0){
        printf("\ngruppo numero %d , il voto del gruppo è %d ,dimensione del gruppo = %d , dimensione desiderata %d, il gruppo di %d è chiuso ? %d", i , shmpB[i].max_vote,shmpB[i].size, shmpB[i].leader_willsize , shmpB[i].group_leader_id ,shmpB[i].closed );
        if(shmpB[i].closed==true){
            if(shmpB[i].leader_willsize != shmpB[i].size){
                shmpB[i].max_vote = shmpB[i].max_vote -3;
            }
            vote_counter = vote_counter + (shmpB[i].max_vote * shmpB[i].size);

        }else{
            shmpB[i].max_vote = 0;
            vote_counter = vote_counter + 0;
        }
        students_n =students_n + shmpB[i].size;
        i++;
    }
    i=0;
    while( i < POP_SIZE){
        kill(shmp[i].id,SIGCONT);
        i++;
    }
    medium_vote = vote_counter/students_n;
    printf("\n ci sono %d studenti nei gruppi\n il voto medio dei gruppi è %d", students_n, medium_vote);
}

void stop_child(struct Students * shmp){
    
    int i=0;
    sleep(sim_time);
    while(i< POP_SIZE){
        if(kill(shmp[i].id,SIGUSR1 /*SIGINT*/)==-1){
            printf("\nERRORE sigint");
        }
        i++;
    }
}




int seminit(){
    int semId;
    
    
    semId = semget(KEY, 1, 0666);               // Check if semkey already exists, if so delete it
    if (semId != -1) {
        semctl(semId, 0, IPC_RMID);
    }

    
    semId = semget(KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);        // Create new semaphore
    if(semId == -1) {
        printf("error on semaphore creation");
        exit(-1);
    }

    
    union semun args;
    args.val = POP_SIZE; 
    semctl(semId, 0, SETVAL, args.val);         // Initialise semaphore
    return semId;
}

int semBinInit(int x){
    int semIdB;
    
    
    semIdB = semget(x, 1, 0666);            // Check if semkey already exists, if so delete it
    if (semIdB != -1) {
        semctl(semIdB, 0, IPC_RMID);
    }

    
    semIdB = semget(x, 1, 0666 | IPC_CREAT | IPC_EXCL);                // Create new semaphore
    if(semIdB == -1) {
        printf("error on semaphore creation");
        exit(-1);
    }

    
    union semun argsB;              // Initialise semaphore
    argsB.val = 0; 
    semctl(semIdB, 0, SETVAL, argsB.val);
    return semIdB;
}




int main(int argc, char** argv){

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
    int st_vote_counter=0;
    int st_medium_vote=0;
    
    if((shmId = shmget(shmkey ,0 , 0)) != -1){
        clean = shmctl(shmId,IPC_RMID, NULL);
        printf("pulisco una vecchia SHM, funzione = %d", clean);
    }
    if((shmIdB = shmget(shmkeyB ,0 , 0)) != -1){
        clean = shmctl(shmIdB,IPC_RMID, NULL);
        printf("pulisco una vecchia SHM, funzione = %d", clean);
    }
    
    if((msgqI = msgget(msgkey , 0666 ))!=-1){
        printf("pulisco una vecchia MSGQ");
        msgctl(msgqI , IPC_RMID, NULL);
    }
    if((msgqR = msgget(msgkeyReply , 0666 ))!=-1){
        printf("pulisco una vecchia MSGQ");
        msgctl(msgqR , IPC_RMID, NULL );
    }
    msgqI = msgget (msgkey , IPC_CREAT | 0666);
    printf("\n msgI = %d", msgqI);
    if(msgqI== -1){
        printf("errore");
        exit(-1);
    }
    msgqR = msgget (msgkeyReply, IPC_CREAT | 0666);
    printf("\n msgR = %d", msgqR);
    if(msgqR == -1){
        printf("errore");
        exit(-1);
    }
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

   

    shmctl(shmId,IPC_SET, buf);
    while(t < POP_SIZE){
        printf("\nSHM figlio con pid : %d , voto %d , stato di gruppo = %d e dimensione desiderata %d , turno %d", shmp[t].id , shmp[t].vote, shmp[t].group , shmp[t].groupSize,shmp[t].turn);
        st_vote_counter = st_vote_counter+shmp[t].vote;
        t++;
    }
    st_medium_vote = st_vote_counter/POP_SIZE;
    print_groups(shmpB, shmp);
    printf("\nvoto medio degli studenti = %d",st_medium_vote);
    printf ("\n   in vero in print true = %d" , true);
    if(shmctl(shmId,IPC_RMID, NULL)==-1){
        printf("\n error on shm remove");
        exit(-1);
    } 
    for(int i=0;i<POP_SIZE;i++){ //waiting sons 
     wait(NULL);
    }  
    msgctl(msgqI , IPC_RMID, NULL);
    msgctl(msgqR , IPC_RMID, NULL);
    shmdt(shmp);
    shmdt(shmpB);
    shmctl(shmId,IPC_RMID, NULL);
    shmctl(shmIdB,IPC_RMID, NULL); 
    return 0;
}