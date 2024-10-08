#define _XOPEN_SOURCE 700

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
int reindeers_waiting = 0;
int presents_delivered = 0;

void* reindeer_func() {
    while (presents_delivered < 4) {
        int sleeping_time = (int) (rand() % 6 + 5);
        printf("Renifer: Bede spal przez %d sekund, id: %ld\n", sleeping_time, pthread_self());
        sleep(sleeping_time);

        pthread_mutex_lock(&mut);
        reindeers_waiting++;
        printf("Renifer: Czeka %d reniferow na Mikolaja, id: %ld \n", reindeers_waiting, pthread_self());
        if (reindeers_waiting == 9) {
            printf("Renifer: Wybudzam Mikolaja %ld\n", pthread_self());
            pthread_cond_signal(&cond);
        }
        pthread_cond_wait(&cond2, &mut);
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}

void* santa_func() {
    int delivering_time;
    while (presents_delivered < 4) {
        pthread_mutex_lock(&mut);
        while (reindeers_waiting < 9) {
            pthread_cond_wait(&cond, &mut);
        }
        printf("Mikolaj: Budze sie\n");
        delivering_time = rand() % 3 + 2;
        printf("Mikolaj: Dostarczam prezenty przez %d sekundy\n",delivering_time);
        sleep(delivering_time);
        presents_delivered++;
        reindeers_waiting = 0;
        printf("Mikolaj: Zasypiam\n");
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}

int main(int argc, char* argv[]) {

    if (argc != 1) {
        printf("Wrong number of arguments\n");
    }

    int reindeer_num = 9;
    pthread_t threads[reindeer_num];
    pthread_t santa;

    srand(time(NULL));

    for (int i = 0; i < reindeer_num; i++) {
        pthread_create(&threads[i], NULL, reindeer_func, NULL);
    }
    pthread_create(&santa, NULL, santa_func, NULL);

    for (int i = 0; i < reindeer_num; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(santa, NULL);
    return 0;
}