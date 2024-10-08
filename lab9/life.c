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

int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

char *foreground = create_grid();
    if (foreground == NULL) {
        endwin();
        fprintf(stderr, "Failed to create foreground grid.\n");
        return -1;
    }

    char *background = create_grid();
    if (background == NULL) {
        endwin();
        fprintf(stderr, "Failed to create background grid.\n");
        destroy_grid(foreground);
        return -1;
    }

    pthread_t *cluster = create_cluster();
    if (cluster == NULL) {
        endwin();
        fprintf(stderr, "Failed to create cluster.\n");
        destroy_grid(foreground);
        destroy_grid(background);
        return -1;
    }

	ThreadArgs *thread_args = create_thread_args();
	char *tmp;

	init_grid(foreground);
	init_cluster(cluster,thread_args,foreground,background);


	while (true)
	{


		draw_grid(foreground);
		usleep(500 * 1000);


		// Step simulation
		update_grid(cluster);
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
