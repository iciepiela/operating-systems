#pragma once
#include <pthread.h>
#include <stdbool.h>

#define NUM_THREADS 4
#define GRID_WIDTH 30
#define GRID_HEIGHT 30

typedef struct {
    char *grid1;
    char *grid2;
    int start_row;
    int end_row;
} ThreadArgs;

char *create_grid();
pthread_t *create_cluster();
ThreadArgs *create_thread_args();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
void init_cluster(pthread_t *threads, ThreadArgs *thread_args, char *foreground, char *background);
bool is_alive(int row, int col, char *grid);
void update_grid(pthread_t *cluster);
void *thread_handler(void *arg);
