#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "semops.h" // my header
#include <stdbool.h>



int main(int argc, char** argv){
    struct Students *shmp;//pointer to the shared memory
    int shmId;
    int i =0;
    int semB;
    struct sembuf* reduceOp = generateReduceOp();
    struct sembuf* waitOp = generateWaitOp();
    struct sembuf* increaseOp = generateIncreaseOp();
    struct Students std;
    if(getpid()%2 == 0){
        std.turn = 1;
    }else{
        std.turn = 2;
    }
    std.id = getpid();
    std.vote = (rand()% 13)+18;
    std.groupSize = 4;

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
    //struct sembuf* waitOp = generateWaitOp();
   // struct sembuf* increaseOp = generateIncreaseOp();
   semop(semB,waitOp,1);
   semop(semB,increaseOp,1);
    while( /*shmp[i] != NULL  && */shmp[i].id != 0){
        printf("\nsono nel while, i vale : %d\n", i);
        i++;
    }
    printf("\n  il valore di shmId student è %d\n", shmId);
    shmp[i] = std;
    printf("\n scrivo nella shm %d\n", shmp[i].id);
    semop(semB,reduceOp,1);

    int turno;
    int votoAE = (rand()% 13)+18;                                   //copie di dati, dovrò eliminarle
    int sem = semget(KEY, 1, 0666); 
    if(getpid()%2 == 0){
        turno = 1;
    }else{
        turno = 2;
    }


    semop (sem , reduceOp , 1);
    semop(sem , waitOp, 1);                //w8tinf for other processes
   // printf(" sono il figlio [pid] %d , voto AE[%d], sono nel turno T%d\n", getpid(), votoAE, turno);

   /* while(true){//ciclo fino a che non ricevo un segnale di interrupt -> fine timer o accetto un invito
        if(shmp[i].vote>25){
            //segnale invito a quelli dello steso turno
        } else{
            //ascolto per inviti 
        }
    } */
}