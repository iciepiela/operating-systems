#define _POSIX_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void SIGUSR_handler(){
    printf("Recived signal SIGUSR :))\n");
}

int main(int argc,char *argv[]){
    if (argc<2){
        printf("Wrong number of arguments");
        return -1;
    }

    char *task = argv[1];
    

    if (strcmp(task,"none")==0){
        signal(SIGUSR1,SIG_DFL);

    }

    else if (strcmp(task,"handler")==0){
        signal(SIGUSR1,SIGUSR_handler);

    }

    else if (strcmp(task,"ignore")==0){
        signal(SIGUSR1,SIG_IGN);

    }
    else if (strcmp(task,"mask")==0){
        
        sigset_t sigmask;
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGUSR1);


        sigprocmask(SIG_SETMASK, &sigmask, NULL);

        raise(SIGUSR1);

        sigset_t pending_signals;
        sigpending(&pending_signals);

        if (sigismember(&pending_signals, SIGUSR1)){
             printf("SIGUSR1 is pending ._. \n");
        }
        else{
            printf("SIGUSR1 is not pending!! \n");
        }
    }

    else{
        printf("Illigal argument. Use one of arguments [mask/handler/ignore/none]\n");
        return -1;
    }
    
    raise(SIGUSR1);
    return 0;
}