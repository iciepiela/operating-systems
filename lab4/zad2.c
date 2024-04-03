#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int global_val = 0;

int main(int argc,char *argv[]){
    int local_val = 0;
        if (argc =! 2){
        printf("Wrong number of arguments");
        return -1;
    }

    pid_t c_pid = fork();
    if (c_pid>0){
        printf("parent process");
        global_val++;
        local_val++;
        printf("parent pid = %d, child pid = %d,",(int)getpid(),(int)c_pid);
        printf("parent's local = %d, parent's global = %d",local_val,global_val);
        wait();

    }
    else{
        printf("child process");
        global_val++;
        local_val++;
        printf("child pid = %d, parent pid = %d",(int)getpid(),(int)getppid());
        printf("child's local = %d, child's global = %d",local_val,global_val);
        execl("/bin/ls",argv[1]);
    }
}