#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define min(a,b) (a<b ? a:b)


double func(double x){
    return 4/(x*x+1);
}

double calc(double start, double end, double size_of_rect){
    double sum = 0.0;
    double x = start;

    while (x < end){
        double step = min(x + size_of_rect, end) - x;
        sum += func(x) * step;
        x += step;
    }
    return sum;
}

int main(int argc, char *argv[]){

    if (argc<3){
        printf("Wrong number of arguments\n");
        return -1;
    }

    clock_t start_time = clock();

    double size_of_rect = strtod(argv[1],NULL);
    int n = atoi(argv[2]);
    double total_time;

    printf("%f %d\n",size_of_rect,n);

    int fd[n][2];

    for (int i = 0;i<n;i++){
        if (pipe(fd[i])<0){
            printf("failed to make a pipe! \n");
        }

        if(fork()==0){
            close(fd[i][0]);
            double start = (double)i / n;
            double end = (double)(i + 1) / n;
            double partial_sum = calc(start, end, size_of_rect);
            write(fd[i][1],&partial_sum,sizeof(partial_sum));
            return 0;
        }
        close(fd[i][1]);

    }

    double sum = 0.0;
    double partial_sum;
    for (int i = 0; i<n;i++){
        read(fd[i][0],&partial_sum,sizeof(partial_sum));
        sum += partial_sum;
    }
    clock_t end_time = clock();

    printf("calculation result: %f\n",sum);

        for (int i = 0; i < n; i++){
        wait(NULL);
    }


    total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Total time taken: %f seconds\n", total_time);

    return 0;
}