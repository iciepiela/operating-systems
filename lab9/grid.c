#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <pthread.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include "grid.h"



void void_handler(int signo){}

char *create_grid()
{
    return malloc(sizeof(char) * GRID_WIDTH * GRID_HEIGHT);
}

pthread_t *create_cluster()
{
    return malloc(sizeof(pthread_t) * NUM_THREADS);
}

ThreadArgs *create_thread_args(){
    return malloc(sizeof(ThreadArgs)*NUM_THREADS);
}

void destroy_grid(char *grid)
{
    free(grid);
}
void *thread_handler(void *arg){
    ThreadArgs *thread_args = (ThreadArgs *)arg;
    char *foreground = thread_args->grid1;
    char *background = thread_args->grid2;
    char *tmp; 
    while (true) {
        pause();

        for (int i = thread_args->start_row; i < thread_args->end_row; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                background[i * GRID_WIDTH + j] = is_alive(i, j, foreground);
            }
        }
        tmp = foreground;
		foreground = background;
		background = tmp;

    }
    return NULL;

}

void draw_grid(char *grid)
{
    for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < GRID_WIDTH; ++j)
        {
            if (grid[i * GRID_WIDTH + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
        grid[i] = rand() % 2 == 0;
}

void init_cluster(pthread_t *cluster, ThreadArgs *thread_args, char *foreground, char *background) {
    int rows_per_thread = GRID_HEIGHT / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_args[i].grid1 = foreground;
        thread_args[i].grid2 = background;
        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == NUM_THREADS - 1) ? GRID_HEIGHT : (i + 1) * rows_per_thread;
        pthread_create(&cluster[i], NULL, thread_handler, &thread_args[i]);
    }

    struct sigaction sa;
	sa.sa_handler = void_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGUSR1, &sa, NULL);

}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= GRID_HEIGHT || c < 0 || c >= GRID_WIDTH)
            {
                continue;
            }
            if (grid[GRID_WIDTH * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * GRID_WIDTH + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}


void update_grid(pthread_t *cluster)
{
    for (int i = 0; i < NUM_THREADS;i++){
            pthread_kill(cluster[i], SIGUSR1);
        
    }
}