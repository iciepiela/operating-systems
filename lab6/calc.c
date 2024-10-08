#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>



#define IN_FIFO_PATH "/tmp/input_fifo"
#define OUT_FIFO_PATH "/tmp/output_fifo"

double func(double x){
    return 4/(x*x+1);
}

double calc(double start, double end, double n){
    double sum = 0.0;
    double x = start;

    double h = (end - start)/n;

    for(int i = 0;i<n;i++){
        sum+=func(x);
        x+=h;
    }

    return sum*h;
}

int main(){
    unlink(IN_FIFO_PATH);
    unlink(OUT_FIFO_PATH);

    char start[10], stop[10];

   
    if (mkfifo(IN_FIFO_PATH, 0666) != 0) {
        printf("Failed to create input pipe\n");
        return -1;
    }
    if(mkfifo(OUT_FIFO_PATH, 0666)!=0){
        printf("Failed to create output pipe\n");
        return -1;
    }

    int input_pipe = open(IN_FIFO_PATH, O_RDONLY);
    int output_pipe = open(OUT_FIFO_PATH, O_WRONLY);

    double result;

    while(read(input_pipe, start, sizeof(start))>0){;
    read(input_pipe, stop, sizeof(stop));
    result = calc(strtod(start,NULL),strtod(stop,NULL),1000);
    write(output_pipe,&result,sizeof(result));
    
    }
    
    close(input_pipe);
    close(output_pipe);
    unlink(IN_FIFO_PATH);
    unlink(OUT_FIFO_PATH);


    return 0;
}