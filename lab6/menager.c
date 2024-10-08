#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



#define IN_FIFO_PATH "/tmp/input_fifo"
#define OUT_FIFO_PATH "/tmp/output_fifo"

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    char start[10], stop[10];
    double result;




    int input_pipe = open(IN_FIFO_PATH, O_WRONLY);
    int output_pipe = open(OUT_FIFO_PATH, O_RDONLY);

    printf("Podaj początek przedziału: ");
    scanf("%s", start);
    
    if(write(input_pipe, start, strlen(start)+1)<0){
        printf("Faliled to write start to input pipe\n");
        return -1;
    }

    printf("Podaj koniec przedziału: ");
    scanf("%s", stop);


    if(write(input_pipe, stop, sizeof(stop))<0){
        printf("Faliled to write stop to input pipe\n");
        return -1;
    }



    if(read(output_pipe,&result,sizeof(result))<0){
        printf("Failed to read reasult from pipe\n");
        return -1;
    }

    printf("result: %f\n",result);

    close(output_pipe);
    close(input_pipe);


    return 0;
}