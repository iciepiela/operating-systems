#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "specs.h"

bool server_running = true;

void shoutdown_handler(int signum){
    server_running = false;
}

int main(){

    struct mq_attr attributes ={
        .mq_flags = 0,
        .mq_msgsize = sizeof(message),
        .mq_maxmsg = 10
    };

    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attributes);
    printf("Server queue: %d\n", server_queue);
    if (server_queue < 0){
        printf("Server not created\n");
        return 1;
    }
    mqd_t clients_queues[MAX_CLIENTS_COUNT];
    for (int i = 0; i < MAX_CLIENTS_COUNT; i++){
        clients_queues[i] = -1;
    }
    int last_client_id = -1;
    message received_message ={
        .type = NONE,
        .id = -1,
        .message_text = ""
    };

    for (int i = 1; i < SIGRTMAX; i++){
        signal(i, shoutdown_handler);
    }

    while (server_running) {
        if (mq_receive(server_queue, (char*)&received_message, sizeof(received_message), NULL)<0){
            printf("failed to receive message");
        }
        switch (received_message.type) {
            case INIT:
                printf("Client registered\n");
                last_client_id ++;
                if (last_client_id == MAX_CLIENTS_COUNT) {
                    printf("Max number of clients reached\n");
                    continue;
                }
                printf("%s\n",received_message.message_text);
                mqd_t new_queue = mq_open(received_message.message_text, O_RDWR, S_IRUSR | S_IWUSR, NULL);
                if (new_queue<0){
                    printf("failed to open client queue\n");
                }

                clients_queues[last_client_id] = new_queue;

                message return_message = {
                    .type = INIT,
                    .id = last_client_id,
                    .message_text = ""
                };
                if (mq_send(clients_queues[last_client_id], (char*)&return_message, sizeof(return_message), 10)<0){
                    printf("failed to send init message\n");
                }
                printf("Send new Id [%d] to client \n",last_client_id);
                break;
            case MESSAGE:
                printf("Message received\n");
                for (int i = 0; i < MAX_CLIENTS_COUNT; i++){
                    if (i != received_message.id){
                        mq_send(clients_queues[i], (char*)&received_message, sizeof(received_message), 10);
                    }
                }
                break;
            default:
                continue;
                break;
        }
    }
    for (int i = 0; i < MAX_CLIENTS_COUNT; i++){
        mq_close(clients_queues[i]);
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}