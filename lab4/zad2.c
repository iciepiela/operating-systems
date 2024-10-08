#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int global_val = 0;

int main(int argc,char *argv[]){
    int local_val = 0;
        if (argc < 2){
        printf("Wrong number of arguments \n");
        return -1;
    }

    pid_t c_pid = fork();
    if (c_pid==0){
        printf("child process: \n");
        global_val++;
        local_val++;
        printf("child pid = %d, parent pid = %d \n",(int)getpid(),(int)getppid());
        printf("child's local = %d, child's global = %d \n",local_val,global_val);
        int error_code = execl("/bin/ls", "ls", "-l", argv[1], NULL);
        return error_code;

    }
        int error_code;
        wait(&error_code);
    
        printf("parent process: \n");
        printf("parent pid = %d, child pid = %d \n",(int)getpid(),c_pid);
        printf("parent's local = %d, parent's global = %d \n",local_val,global_val);
        printf("child's exit code: %d\n",WEXITSTATUS(error_code));
        return WEXITSTATUS(error_code);
    
}