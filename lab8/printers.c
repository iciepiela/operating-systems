#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <semaphore.h>

#define SHM_NAME "shared_memory_name"
#define PRINTER_SEM_NAME "printer_semaphore_name"
#define USER_SEM_NAME "user_semaphore_name"
#define MEM_SEM_NAME "memory_semaphore_name"




int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Wrong number of arguments\n");
        return -1;
    }
    
    sem_unlink(PRINTER_SEM_NAME);
    sem_unlink(USER_SEM_NAME);
    sem_unlink(MEM_SEM_NAME);


    shm_unlink(SHM_NAME);

    int num_printers = atoi(argv[1]);


    sem_t* sem_printer = sem_open(PRINTER_SEM_NAME, O_CREAT | O_EXCL, 0664, num_printers);
    if (sem_printer == SEM_FAILED) {
        printf("Failed to create semaphore\n");
        return -1;
    }
    sem_t* sem_users = sem_open(USER_SEM_NAME, O_CREAT | O_EXCL, 0664, 0);
    if (sem_users == SEM_FAILED) {
        printf("Failed to create semaphore\n");
        return -1;
    }

    sem_t* sem_mem = sem_open(MEM_SEM_NAME, O_CREAT | O_EXCL, 0664, 1);
    if (sem_mem == SEM_FAILED) {
        printf("Failed to create semaphore\n");
        return -1;
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    if (shm_fd<0){
        printf("Failed to open shared memory\n");
        return -1;
    }

    if (ftruncate(shm_fd, 4096)<0){
        printf("Failed to truncate\n");
        return -1;
    }

    char* shm_map = mmap(0, 4096, PROT_READ| PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i=0;i<num_printers;i++){
        if (fork()==0){
            printf("[Printer %d] is ready\n", getpid());



            while (true) {
                sem_wait(sem_printer);
                sem_wait(sem_users);
                sem_wait(sem_mem);

                char* shared_data =shm_map;
                printf("[Printer %d] \n",getpid());
                for (int j = 0; j < 10; j++) {
                    printf("%c\n",shared_data[j]);
                    fflush(stdout);
                    sleep(1); 
                }
                printf("\n");

                fflush(stdout);
                sem_post(sem_printer);
                sem_post(sem_mem);
                sleep(1);
            }


            return 0;
        }
    }


    while(wait(NULL) > 0);

    sem_close(sem_printer);
    sem_close(sem_users);
    sem_close(sem_mem);


    sem_unlink(PRINTER_SEM_NAME);
    sem_unlink(USER_SEM_NAME);
    sem_unlink(MEM_SEM_NAME);


    shm_unlink(SHM_NAME);
    return 0;

}

