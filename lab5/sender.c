#define _XOPEN_SOURCE 500

#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void SIGUSR_handler(){
    printf("Recived confirmation from catcher\n");
}

int main(int argc,char *argv[]){

    if (argc<3){
        printf("Wrong number of arguments\n");
        return -1;
    }

    printf("Sender PID: %d\n",(int)getpid());
    signal(SIGUSR1,SIGUSR_handler);

    int catcher_pid = atoi(argv[1]);
    int sender_argument = atoi(argv[2]);

    const union sigval sender_argument_val = {sender_argument};

    sigqueue(catcher_pid, SIGUSR1, sender_argument_val);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    sigsuspend(&mask);

    return 0;

}