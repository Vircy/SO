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




int main(int argc, char** argv){
    struct shmsg* shmp;//pointer to the shared memory

    shmp = (struct shmseg*)shmat(shmkey, NULL, 0);
        if(shmp == (void*)-1 ){
            printf("errore nel collegamento della SHM in student");
            exit(-1);
        }

    srand(getpid());
    int turno;
    int votoAE = (rand()% 13)+18;
    int sem = semget(KEY, 1, 0666); //atoi(argv[0]); 
    printf("semaforo sem %d" , sem);
    perror("\nTestFiglio");
    struct sembuf* reduceOp = generateReduceOp();
    struct sembuf* waitOp = generateWaitOp();
    //perror("\nGenerated wait ops");
    if(getpid()%2 == 0){
        turno = 1;
    }else{
        turno = 2;
    }
    perror("\nExecuting reduce ops");
    semop (sem , reduceOp , 1);
    perror("\nExecuting wait ops");
    semop(sem , waitOp, 1);
    perror("\nDone waiting");
    printf(" sono il figlio [pid] %d , voto AE[%d], sono nel turno T%d\n", getpid(), votoAE, turno);
}