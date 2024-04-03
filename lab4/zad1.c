#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc,char *argv[]){
    if (argc =! 2){
        printf("Wrong number of arguments");
        return -1;
    }
    pid_t c_pid;

    for(int i = 0;i<(int)&argv[1];i++){
        c_pid = fork();
        if (c_pid > 0){
            printf("dziecko nr %d PPID: %d PID:%d",i,(int)getppid(),(int)getpid());
        }
    }
    if (c_pid == 0){
        wait(NULL);
    }
    return 0;
}