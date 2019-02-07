
#include "semops.h" // my header
bool working = true;


int ceckReply(){
    return 0;
}
void sig_handler(){
        working = false;
}
int group_size(){
    int two,tree,four,random;
    FILE *file;
    file = fopen("config.txt","r");
    if(file == NULL){
        exit(-1);
    }
    random = (rand()%100);
    fscanf(file,"%d %d %d", &two,&tree ,&four);
    if(random<=two){
        return 2;
    }else if(random<= (two+tree)){
        return 3;
    }else{
        return 4;
    }
}


int set_group(struct Groups *shmpB, int j,struct MyReplys myReply){
    bool working = true;
    shmpB[j].size += 1; 
    if(shmpB[j].size == shmpB[j].leader_willsize){
        shmpB[j].closed = true;
        working = false;
    }
        if(myReply.vote > shmpB[j].max_vote){
            shmpB[j].max_vote = myReply.vote;
    }
    return working;
}

int main(int argc, char** argv){
    int two,tree,four;
    struct Students *shmp;
    struct Groups *shmpB;
    int msgInvite;
    int msgReply;
    int shmId;
    int shmIdB;
    int i=0;
    int semB;
    int semTwo;
    struct sembuf* reduceOp = generateReduceOp();
    struct sembuf* waitOp = generateWaitOp();
    struct sembuf* increaseOp = generateIncreaseOp();
    struct Students std;
    struct MyReplys myReply;
    struct MyInvites myInvite;
    struct Groups myGroup;
    if(signal(SIGUSR1, sig_handler)==SIG_ERR){
        printf("\ncan't catch signal\n");
    }
    srand(clock());

    if(getpid()%2 == 0){
        std.turn = 1;
    }else{
        std.turn = 2;
    }
    std.id = getpid();
    std.vote = (rand()% 13)+18;
    std.groupSize = group_size(two,tree, four);
    std.group = false;
    msgInvite = msgget(msgkey ,0);
    if(msgInvite == -1 ){
        printf("error opening the MSGQ from student");
        exit(-1);
    }
    msgReply = msgget(msgkeyReply ,0 );
    if(msgReply  == -1 ){
        printf("error opening the MSGQ from student");
        exit(-1);
    }
    semTwo = semget(KEYTWO , 1 ,0666);
    semB = semget(KEYBIN, 1, 0666);
    shmId = shmget(shmkey ,0 , 0);
    shmIdB = shmget(shmkeyB ,0 , 0);
    if( shmId == -1 ){
            printf("\n errore nel collegamento della SHM in student");
            exit(-1);
        }
        if( shmIdB == -1 ){
            printf("\n errore nel collegamento della SHM in student");
            exit(-1);
        }

    shmp = ( struct Students*)shmat(shmId, NULL, 0);
        if(shmp == (void*)-1 ){
            printf("errore nell apertura della SHM in student");
            exit(-1);
        }
    shmpB = ( struct Groups*)shmat(shmIdB, NULL, 0);
        if(shmpB == (void*)-1 ){
            printf("errore nell apertura della SHM in student");
            exit(-1);
        }
   semop(semB,waitOp,1);
   semop(semB,increaseOp,1);
    while( shmp[i].id != 0){
        i++;
    }
    shmp[i] = std;
    semop(semB,reduceOp,1);

    int turno;
    int votoAE = (rand()% 13)+18;
    int sem = semget(KEY, 1, 0666); 
    if(getpid()%2 == 0){
        turno = 1;
    }else{
        turno = 2;
    }

       
    semop (sem , reduceOp , 1);
    semop (sem , waitOp, 1);                //w8ting for other processes
    int my_group_lead=0;
    int counter=(i+1)%POP_SIZE;
    int maxInvites = (shmp[i].groupSize) -1;
    bool leader = false;
    bool group_member = false;
    bool setDone = false;
    bool no_invite = false;
    bool no_invite_grup =false;
    int j = 0;
    while(working == true){
        while(msgrcv(msgReply , &myReply, sizeof(struct MyReplys), getpid(),IPC_NOWAIT) != -1 ){  //    checkReply
            if(myReply.reply == true){
                leader = true;
                semop(semTwo, waitOp,1);
                semop(semTwo , increaseOp,1);
                if(setDone == false){
                    while(shmpB[j].max_vote != 0){
                        j++;
                    }
                    shmpB[j].group_leader_id = shmp[i].id;
                    shmpB[j].max_vote = shmp[i].vote;
                    shmpB[j].leader_willsize = shmp[i].groupSize;
                    shmpB[j].size += 1;
                    setDone = true;
                    semop(semB,waitOp,1);
                    semop(semB,increaseOp,1);
                    shmp[i].group = true;
                    semop(semB,reduceOp,1);

                }
                working = set_group(shmpB, j, myReply);
                semop(semTwo,reduceOp,1);
               
            }else if(myReply.reply == false){
                maxInvites = maxInvites+1;
            }
            
        }
        while(msgrcv(msgInvite , &myInvite, sizeof(struct MyInvites), getpid(),IPC_NOWAIT) != -1){  // ceckInvites
                if(myInvite.willsize == shmp[i].groupSize  && shmp[i].group == false && maxInvites == (shmp[i].groupSize-1)){
                    myReply.from = getpid();
                    myReply.mtype = myInvite.from;
                    myReply.reply = true;
                    myReply.vote = shmp[i].vote;
                    myReply.willSize = shmp[i].groupSize;
                    semop(semB,waitOp,1);
                    semop(semB,increaseOp,1);
                    shmp[i].group = true; 
                    semop(semB,reduceOp,1);
                    group_member = true;
                    maxInvites = 0;
                    if(msgsnd(msgReply,&myReply,sizeof(struct MyReplys),0) == -1){//send Accept
                        printf("\n error sendin a message inside ceckInvites ( OK)");
                        exit(-1);
                    } 
                    my_group_lead=myInvite.from;
                    working = false;
                }else{
                    myReply.from = getpid();
                    myReply.mtype = myInvite.from;
                    myReply.reply = false;
                    myReply.vote = shmp[i].vote;
                    myReply.willSize = shmp[i].groupSize;
                    if(msgsnd(msgReply,&myReply,sizeof(struct MyReplys),0)== -1){//send refuse
                        printf("\n error sendin a message inside ceckInvites (NOPE)");
                        exit(-1);
                    }
                }
            }
          
          
            while(counter!= i&& maxInvites>0 && shmp[i].group== false ){//send invites
                no_invite = true;
                if(i != counter){
                    if(shmp[counter].group == false && shmp[counter].turn == shmp[i].turn /*&& shmp[counter].vote>shmp[i].vote*/){
                        if(shmp[counter].groupSize == shmp[i].groupSize){//pick a group mate fom SHM
                            no_invite = false;
                            myInvite.from=getpid();
                            myInvite.mtype=shmp[counter].id;
                            myInvite.vote= shmp[i].vote;
                            myInvite.willsize=shmp[i].groupSize;
                            maxInvites--;
                            if(msgsnd(msgInvite,(void *)&myInvite,sizeof(struct MyInvites),0)== -1){
                                printf("\nerror sendin a message on the invites Q");
                                exit(-1);
                            }
                        }
                    }
                }
                counter = (counter+1)%POP_SIZE;
            }
            counter = i+1;
            while(counter!= i  && maxInvites>0 && leader == true && shmpB[j].closed == false){//send invites
            no_invite = true;
                if(i != counter){
                    if(shmp[counter].group == false && shmp[counter].turn == shmp[i].turn && shmp[counter].groupSize == shmp[i].groupSize){      
                            no_invite  = false;
                            myInvite.from=getpid();
                            myInvite.mtype=shmp[counter].id;
                            myInvite.vote= shmp[i].vote;
                            myInvite.willsize=shmp[i].groupSize;
                            maxInvites--;
                            if(msgsnd(msgInvite,(void *)&myInvite,sizeof(struct MyInvites),0)== -1){
                                printf("\nerror sendin a message on the invites Q");
                                exit(-1);
                            }
                        }
                }
               counter = (counter+1)%POP_SIZE;
            }
        
            if(no_invite == true && maxInvites>0 ){
                if(leader == true && ((shmpB[j].size+maxInvites) == shmpB[j].leader_willsize)){
                    semop(semTwo, waitOp,1);
                    semop(semTwo , increaseOp,1);
                    shmpB[j].closed = true;
                    working = false;
                    semop(semTwo,reduceOp,1);
                }else if(leader == false){
                    leader = true;
                    semop(semTwo, waitOp,1);
                    semop(semTwo , increaseOp,1);
                    while(shmpB[j].max_vote != 0){
                        j++;
                    }
                    shmpB[j].group_leader_id = shmp[i].id;
                    shmpB[j].max_vote = shmp[i].vote;
                    shmpB[j].leader_willsize = shmp[i].groupSize;
                    setDone = true;
                    semop(semB,waitOp,1);
                    semop(semB,increaseOp,1);
                    shmp[i].group = true;
                    semop(semB,reduceOp,1);
                    set_group(shmpB, j, myReply);
                    semop(semTwo,reduceOp,1);

                }
            }
         
    }
    kill( getpid(),SIGSTOP);
    if(leader==true){
        printf("\nsono %d , il mio voto di SO è %d\n", getpid() , shmpB[j].max_vote);
    }else{
        i=0;
        while(shmpB[i].group_leader_id != my_group_lead && shmpB[i].group_leader_id!= 0){
            i++;
        }
        printf("\nsono %d , il mio voto di SO è %d ,leader:%d \n", getpid() , shmpB[i].max_vote , my_group_lead);
    }
    if(shmdt(shmp)== -1){
        printf("\nerror detatching students shm");
        exit(-1);
    }
    if(shmdt(shmpB)== -1){
        printf("\nerror detatching groups shm");
        exit(-1);
    }

    return 0;
}
