#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    if (argc < 2){
        printf("Wrong number of arguments \n");
        return -1;
    }
    int num_of_processes = atoi(argv[1]);
    pid_t c_pid;

    for(int i = 0;i<num_of_processes;i++){
        c_pid = fork();
        if (c_pid == 0){
            printf("Child nr %d PPID: %d PID:%d \n",i,(int)getppid(),(int)getpid());
            return 0;
        }
    }
    
    while (wait(NULL)>0);
    printf("Number of processes: %d \n",num_of_processes);
    return 0;
}