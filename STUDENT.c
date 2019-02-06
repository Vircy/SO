#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdbool.h>
#include "semops.h" // my header
bool working = true;


int ceckReply(){
    return 0;
}
void sig_handler(){
    //if(signo == SIGINT){
        //printf("\nsignal received\n");
        working = false;
    //}
}
int group_size(){
    int two,tree,four,random;
    FILE *file;
    file = fopen("config.txt","r");
    if(file == NULL){
        exit(-1);
    }
    random = (rand()%100);
   // printf("randomizzato %d",random);
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
    if(myReply.willSize == shmpB[j].leader_willsize){
        if(myReply.vote > shmpB[j].max_vote){
            shmpB[j].max_vote = myReply.vote;
        }
    }else if((myReply.vote -3)> shmpB[j].max_vote) {
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
        //printf("\nsono nel while, i vale : %d\n", i);
        i++;
    }
    shmp[i] = std;
    //printf("\n scrivo nella shm %d , sono il figlio di pid = %d\n", shmp[i].id , getpid());
    semop(semB,reduceOp,1);

    int turno;
    int votoAE = (rand()% 13)+18;                                   //copie di dati che dovrò eliminarle
    int sem = semget(KEY, 1, 0666); 
    if(getpid()%2 == 0){
        turno = 1;
    }else{
        turno = 2;
    }

       
    semop (sem , reduceOp , 1);
    semop (sem , waitOp, 1);                //w8tinf for other processes

    int counter=(i+1)%POP_SIZE;
    int maxInvites = (shmp[i].groupSize) -1;
    bool leader = false;
    bool group_member = false;
    bool setDone = false;
    bool no_invite = false;
    bool no_invite_grup =false;
   // bool working = true;
    int j = 0;
    while(working == true){
      //  printf("\n sono %d ho %d inviti",getpid() , maxInvites);
        while(msgrcv(msgReply , &myReply, sizeof(struct MyReplys), getpid(),IPC_NOWAIT) != -1 ){  //    checkReply
          // printf("\n cerco risposte , sono %d", getpid());
            if(myReply.reply == true/* && shmp[i].group == false*/){
               //printf("\n new leader !  pid=  %d recived accept from %d, still have %d invites",getpid(),myReply.from , maxInvites);
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
                  //  printf("\n the leader %d has set his group to  %d",getpid(),shmp[i].group);
                    semop(semB,reduceOp,1);

                }
                working = set_group(shmpB, j, myReply);////////////////////////////////////////////////////////
                semop(semTwo,reduceOp,1);
               
            }else if(myReply.reply == false){
              // printf("ricevuto rifiuto, sono %d" ,getpid());
                maxInvites = maxInvites+1;
            }
            
        }
        while(msgrcv(msgInvite , &myInvite, sizeof(struct MyInvites), getpid(),IPC_NOWAIT) != -1){  // ceckInvites
       // printf("\n cerco inviti, sono %d ",getpid());
                if(myInvite.willsize == shmp[i].groupSize  && shmp[i].group == false && maxInvites == (shmp[i].groupSize-1)){
                   // printf("\n found an invite for %d from %d , sending accept" ,getpid() ,myInvite.from);
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
                    working = false;////////////////////////////////////////////////////
                    //printf("\ninvite_accepted\n");
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
                  //  printf("\nsending a refuse to an invit i'm %d",getpid());
                }
            }
          //  counter=(i+1)%POP_SIZE;
          
          
            while(counter!= i&& maxInvites>0 && shmp[i].group== false ){//send invites
                no_invite = true;
                if(i != counter){
                   // printf("\n figlio %d, valore di counter %d",getpid(),counter/*, shmp[i].groupSize ,shmp[counter].groupSize*/);
                    if(shmp[counter].group == false && shmp[counter].turn == shmp[i].turn /*&& shmp[counter].vote>shmp[i].vote*/){
                        if(shmp[counter].groupSize == shmp[i].groupSize){//pick a group mate fom SHM
                            no_invite = false;
                            myInvite.from=getpid();
                            myInvite.mtype=shmp[counter].id;
                            myInvite.vote= shmp[i].vote;
                            myInvite.willsize=shmp[i].groupSize;
                            maxInvites--;
                            if(msgsnd(msgInvite,(void *)&myInvite,sizeof(struct MyInvites),0)== -1){//HO MESSSO IPC_NOWAIT
                                printf("\nerror sendin a message on the invites Q");
                                exit(-1);
                            }
                          //  printf("\n invite sent for %d from %d", shmp[counter].id,shmp[i].id);
                        }
                    }
                }
                counter = (counter+1)%POP_SIZE;
            }
            counter = i+1;
            while(counter!= i  && maxInvites>0 && leader == true && shmpB[j].closed == false){//send invites
            no_invite = true;
                if(i != counter){
                  //  printf("\n leader %d, valore di counter %d, gruppo di dimensione %d ",getpid(),counter, shmpB[j].size/* ,shmp[counter].groupSize*/);
                    if(shmp[counter].group == false && shmp[counter].turn == shmp[i].turn && shmp[counter].groupSize == shmp[i].groupSize){
               
                       // if(/*shmp[counter].groupSize == shmp[i].groupSize && */shmp[counter].turn == shmp[i].turn){//pick a group mate fom SHM         
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
                           // printf("\n invite sent for %d from %d", shmp[counter].id,shmp[i].id);
                        }
                    //}
                }
               counter = (counter+1)%POP_SIZE;
            }
        
            if(no_invite == true && maxInvites>0 ){
                if(leader == true && ((shmpB[j].size+maxInvites) == shmpB[j].leader_willsize)){
                    semop(semTwo, waitOp,1);
                    semop(semTwo , increaseOp,1);
                    shmpB[j].closed = true;
                    working = false;/////////////////////////////////////////////
                   // printf("\nno_invites_found_selfclose\n");
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
                    //printf("\n il voto è : %d ", shmp[i].vote);
                    shmpB[j].leader_willsize = shmp[i].groupSize;
                    setDone = true;
                    semop(semB,waitOp,1);
                    semop(semB,increaseOp,1);
                    shmp[i].group = true;
                   // printf("\n the leader %d has set his group to  %d not finding invites to send",getpid(),shmp[i].group);
                    semop(semB,reduceOp,1);
                    set_group(shmpB, j, myReply);
                    semop(semTwo,reduceOp,1);

                }
            }
         
    }
    /*if(shmdt(shmp)== -1){
        printf("\nerror detatching students shm");
        exit(-1);
    }
    if(shmdt(shmpB)== -1){
        printf("\nerror detatching groups shm");
        exit(-1);
    }*/
   // printf("\nCIAO_ho_finito_il_working, %d \n", getpid());
    //sleep(20);
    return 0;
}
   // printf(" sono il figlio [pid] %d , voto AE[%d], sono nel turno T%d\n", getpid(), votoAE, turno);

   /* while(true){//ciclo fino a che non ricevo un segnale di interrupt -> fine timer o accetto un invito
        if(shmp[i].vote>25){
            //segnale invito a quelli dello steso turno
        } else{
            //ascolto per inviti 
     grouop
    }grouop
    }}op
    /////chiudi il gruppo
    */
