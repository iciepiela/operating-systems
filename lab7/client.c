#define _XOPEN_SOURCE 700

#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#include "specs.h"

bool client_running = true;

void shoutdown_handler(int signum){
    client_running = false;
}

int main(int argc, char* argv[]){

    struct mq_attr attributes ={
        .mq_flags = 0,
        .mq_msgsize = sizeof(message),
        .mq_maxmsg = 10
    };




    char client_queue_name[100] = {0};
    sprintf(client_queue_name, "/client_queue_name_%d", getpid());

    mqd_t client_queue = mq_open(client_queue_name, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attributes);
    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    printf("%d\n", server_queue);
    if (server_queue < 0){
        printf("server not created\n");
        return 1;
    }
    if (client_queue < 0){
        printf("client not created\n");
        return 1;
    return 1;
    }

    message message_to_send = {
        .type = INIT,
        .id = -1,
    };

    memcpy(message_to_send.message_text, client_queue_name, strlen(client_queue_name));
    int a = mq_send(server_queue, (char*)&message_to_send, sizeof(message_to_send), 10);
    if (a < 0){
        printf("Failed to send init message\n");
        return 1;
    } else {
        printf("Init message sent\n");
        printf("client queue name: %s\n", message_to_send.message_text);
    }

    int fd[2];
    pipe(fd);

    for (int i = 1; i < SIGRTMAX; i++){
        signal(i, shoutdown_handler);
    }

    pid_t child_pid = fork();
    if (child_pid == 0){
        close(fd[0]);
        message received_message = {
            .type = NONE,
            .id = -1,
            .message_text = ""
        };

        while(client_running) {
            mq_receive(client_queue, (char*)&received_message, sizeof(received_message), NULL);
            switch (received_message.type){
                case INIT:
                    printf("\nReceived new Id from server [%d]\n", received_message.id);
                    write(fd[1], &received_message.id, sizeof(received_message.id));
                    break;
                case MESSAGE:
                    printf("\nReceived message: %s \n", received_message.message_text);
                    break;
                default:
                    continue;
                    break;
            }
        }
    } else {
        close(fd[1]);
        int id;
        read(fd[0], &id, sizeof(id));
        char* temp;
        while (client_running){
            if(scanf("%ms", &temp) == 1) {
                message message_to_send = {
                    .type = MESSAGE,
                    .id = id
                };

                memcpy(message_to_send.message_text, temp, strlen(temp));
                int a = mq_send(server_queue, (char*)&message_to_send, sizeof(message_to_send), 10);
                if (a < 0){
                    printf("Message not sent--------------------------------\n");
                    printf("text: %s\n", message_to_send.message_text);
                    return 1;
                } else {
                    printf("Message sent\n");
                    printf("text: %s\n", message_to_send.message_text);
                }
                free(temp);
                temp = NULL;
            }
        }
    }

    mq_close(client_queue);
    mq_close(server_queue);
    mq_unlink(client_queue_name);
    return 0;
}