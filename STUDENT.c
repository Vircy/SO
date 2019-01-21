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



int main(int argc, char** argv){
    struct Students *shmp;//pointer to the shared memory
    int msgInvite;
    int msgReply;
    int shmId;
    int i=0;
    int semB;
    struct sembuf* reduceOp = generateReduceOp();
    struct sembuf* waitOp = generateWaitOp();
    struct sembuf* increaseOp = generateIncreaseOp();
    struct Students std;
    struct MyReplys myReply;
    struct MyInvites myInvite;
    if(getpid()%2 == 0){
        std.turn = 1;
    }else{
        std.turn = 2;
    }
    std.id = getpid();
    std.vote = (rand()% 13)+18;
    std.groupSize = 2;
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

    semB = semget(KEYBIN, 1, 0666);
    shmId = shmget(shmkey ,0 , 0);//sizeof(int)*(POP_SIZE)
    if( shmId == -1 ){
            printf("\n errore nel collegamento della SHM in student");
            exit(-1);
        }

    shmp = ( struct Students*)shmat(shmId, NULL, 0);
        if(shmp == (void*)-1 ){
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

    int counter=0;
    int maxInvites = (shmp[i].groupSize) -1;
    printf("\n maxInvites of student = %d", maxInvites);
    bool leader = false;
    for(int maxTotal= 5 ; maxTotal > 0 ; maxTotal--){
      printf("\n valore di maxTotal = %d", maxTotal);
        while(msgrcv(msgReply , &myReply, sizeof(struct MyReplys), getpid(),IPC_NOWAIT) != -1){  //    ceckReply
           printf("\n cerco risposte");
            if(myReply.reply == true){
                leader = true;
                semop(semB,waitOp,1);
                semop(semB,increaseOp,1);
                shmp[i].group = true;
                semop(semB,reduceOp,1);
            }else if(myReply.reply = false){
                maxInvites+1;
            }
            
        }
        while(msgrcv(msgInvite , &myInvite, sizeof(struct MyInvites), getpid(),IPC_NOWAIT) != -1){  // ceckInvites
        printf("\n cerco inviti");
                if(myInvite.group == shmp[i].groupSize  && shmp[i].group == false){
                    myReply.from = getpid();
                    myReply.mtype = myInvite.from;
                    myReply.reply = true;
                    if(msgsnd(msgReply,&myReply,sizeof(struct MyReplys),0)== -1){//send Accept
                        printf("\n error sendin a message inside ceckInvites ( OK)");
                        exit(-1);
                    } 
                    semop(semB,waitOp,1);
                    semop(semB,increaseOp,1);
                    shmp[i].group = true;
                    semop(semB,reduceOp,1);

                }else{
                    myReply.from = getpid();
                    myReply.mtype = myInvite.from;
                    myReply.reply = false;
                    if(msgsnd(msgReply,&myReply,sizeof(struct MyReplys),0)== -1){//send Accept
                        printf("\n error sendin a message inside ceckInvites (NOPE)");
                        exit(-1);
                    }  //send refuse
                }
            }
            while(shmp[counter].vote != 0 && maxInvites>0){//send invites
                   
                if(i != counter){
                    if(shmp[counter].group == false){
                        if(shmp[counter].groupSize == shmp[i].groupSize){//pick a group mate fom SHM
                            myInvite.from=getpid();
                            myInvite.mtype=shmp[counter].id;
                            myInvite.vote= shmp[i].vote;
                            myInvite.group=shmp[i].groupSize;
                            maxInvites--;
                            if(msgsnd(msgInvite,(void *)&myInvite,sizeof(struct MyInvites),0)== -1){
                                printf("\nerror sendin a message on the invites Q");
                                exit(-1);
                            }
                            printf("\n invite sent form %d", shmp[counter].id);
                        }
                    }
                }
                counter++;
            }
              /*  if(maxInvites>0){
                    ///////////NON HO TROVATO PERSONE DA INVITARE MA SONO SOLO  ( MI CHIUDO DA SOLO?)

                }  */
    }
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
