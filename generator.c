#include <unistd.h> 


main{
    int POP_SIZE = 5;
    const char sonfile[8] = "STUDENT";
    chat argv[]= {NULL};

    for(int i=0;i<POP_SIZE;i++) // loop will run n times 
    { 
        if(fork() == 0){  
        
            int execve (sonfile, argv,NULL);
            //printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid()); 
            exit(0); 
        } 
    } 
    for(int i=0;i<5;i++) // loop will run n times (n=5) 
    wait(NULL); 
      
} 