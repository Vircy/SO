#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h> 

int main(){
    srand(getpid());
    int turno;
    int votoAE = (rand()% 13)+18;
    if(getpid()%2 == 0){
        turno = 1;
    }else{
        turno = 2;
    }
    printf(" sono il figlio [pid] %d , voto AE[%d], sono nel turno T%d\n", getpid(), votoAE, turno);
}