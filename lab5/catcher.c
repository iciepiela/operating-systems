#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


int state = 0;
int no_changes=0;


void SIGUSR_handler(int signo, siginfo_t *info, void *context){
    printf("SIGUSR received by catcher with argument %d!\n",info->si_int);
    no_changes++;
    state = info->si_int;
    kill(info->si_pid,SIGUSR1);
}

int main(){

    printf("Catcher PID: %d\n",(int)getpid());

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = SIGUSR_handler;
    sigaction(SIGUSR1, &sa, NULL);

    while(1){
        switch(state){
            case 1:
                for(int i =1;i<11;i++){
                    printf("%d\n",i);
                }
                state = 0;
                break;
            case 2:
                printf("state changed %d numer of times\n",no_changes);
                state = 0;
                break;
            case 3:
                printf("Bye bye!\n");
                return 0;
                state = 0;
                break;
            default:
                break;

        };
    }
    return 0;

}