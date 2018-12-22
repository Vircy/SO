#include <unistd.h> 
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>

int main(){
    int POP_SIZE = 5;
    //const char sonFile[8] = "STUDENT";
    char *argv[] = { NULL };

    for(int i=0;i<POP_SIZE;i++){ // loop will run n times 
     
        printf("\n POP_SIZE : %d" , i);
        if(fork() == 0){  
            printf("\n fork");
            execve ("STUDENT", argv, NULL);
         } 
    } 
    for(int i=0;i<POP_SIZE;i++){ // loop will run n times (n=5) 
     wait(NULL);
    }
      
} 