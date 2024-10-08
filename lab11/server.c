#define _XOPEN_SOURCE 700


#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


#include "specs.h"



bool server_running = true;

void shoutdown_handler(int signum){
    server_running = false;
}

void shoutdown_program(int server_fd){

    shutdown(server_fd,SHUT_RDWR);
    close(server_fd);
}

void build_client_list(client clients[], char *client_list, size_t client_list_size) {
    char temp[512]; 
    size_t used = 0; 
    for (int i = 0; i < MAX_CLIENTS_COUNT; i++) {
        if (clients[i].alive) {
            int written = snprintf(temp, sizeof(temp), "Id: %d, Name: %s\n", clients[i].id, clients[i].name);
            if (used + written < client_list_size) {
                memcpy(client_list + used, temp, written);
                used += written;
            } else {
                break;
            }
        }
    }
    client_list[used] = '\0';
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Wrong number of arguments\n");
        return -1;
    }

    uint16_t port = (uint16_t)strtol(argv[1],NULL,10);

    for (int i = 1; i < SIGRTMAX; i++){
        signal(i, shoutdown_handler);
    }


    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int server_fd;
    int client_fd;
    int client_count = 0;
    int epoll_fd;
    client clients[MAX_CLIENTS_COUNT];
    struct epoll_event events[MAX_CLIENTS_COUNT],new_event;
    message mes;

    
    epoll_fd = epoll_create1(0);
    if (epoll_fd<0){
        printf("Failed to create epoll_fd\n");
        return -1;
    }

    for (int i = 0;i<MAX_CLIENTS_COUNT;i++){
        clients[i].alive=false;
    }

    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd < 0){
        printf("failed to create a socket\n");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        printf("Failed to set options\n");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    

    if (bind(server_fd,(struct sockaddr*)&address,addrlen) < 0){
        shoutdown_program(server_fd);
        printf("Failed to bind a socket\n");
        return -1;
    }
    if (listen(server_fd,MAX_CLIENTS_COUNT) < 0){
        shoutdown_program(server_fd);
        printf("Failed to listen\n");
        return -1;
    }


    new_event.data.fd = server_fd;
    new_event.events = EPOLLIN;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_event.data.fd, &new_event)<0)
    {
        printf("Failed to add server_fd to epoll\n");
        close(epoll_fd);
        return -1;
    }

    if(fork()==0){
        while (server_running){
            message alive_message;
            alive_message.type = ALIVE;
            for (int i = 0; i<MAX_CLIENTS_COUNT;i++){
                if (clients[i].alive){
                    if(write(clients[i].fd,&alive_message,sizeof(message))<0){
                        printf("<Server> Client %d is not active\n",clients[i].id);
                        printf("<Server> Disconnecting client...");
                        clients[i].alive = false;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clients[mes.id].fd, NULL);
                    }
                    
                }
            }
            sleep(5);
        }
        return 0;
    }
    
    
    while (server_running){
        int event_count;
        event_count = epoll_wait(epoll_fd,events,MAX_CLIENTS_COUNT,-1);
        if (event_count<0){
            printf("Failed to epoll_wait\n");
            return -1;
        }
        for (int i = 0;i<event_count;i++){
        if (events[i].data.fd == server_fd){
            client_fd = accept(server_fd,(struct sockaddr*)&address,&addrlen);
            if (client_fd<0){
                shoutdown_program(server_fd);
                printf("Failed to accept\n");
                return -1;
            }
            if(read(client_fd,&mes,sizeof(message))<0){
                shoutdown_program(server_fd);
                printf("Reading init failed\n");
                return -1;
            }
            if (mes.type==INIT){
                printf("<Server> Received INIT message\n");
                if (client_count >= MAX_CLIENTS_COUNT){
                    printf("<Server> Max client count reached!!\n");
                }
                else{
                    int new_client_id = -1;

                    for (int i = 0; i < MAX_CLIENTS_COUNT; i++) {
                        if (clients[i].alive == false) {
                            new_client_id = i;
                            clients[i].alive = true;
                            clients[i].fd = client_fd;
                            clients[i].id = i;
                            strncpy(clients[i].name, mes.message_text, MAX_NAME_LEN - 1);
                                    clients[i].name[MAX_NAME_LEN - 1] = '\0';
                            

                            new_event.data.fd = client_fd;
                            new_event.events = EPOLLIN;

                            if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &new_event)<0)
                            {
                                printf("Failed to add file descriptor to epoll\n");
                                close(epoll_fd);
                                return -1;
                            }
                                
                            break; 
                        }
                    }
                    if (new_client_id != -1) {
                        printf("<Server> New client [%s] initialized with id %d\n", clients[new_client_id].name, new_client_id);
                        printf("<Server> Sending new id to client...\n");
                        mes.type = INIT;
                        mes.id = new_client_id;
                        if (write(clients[new_client_id].fd, &mes, sizeof(message)) < 0) {
                            shoutdown_program(server_fd);
                            printf("Sending init failed\n");
                            return -1;
                        }
                        client_count++;
                    }
                }
            }
            }
            
            else if (events[i].events & EPOLLIN) {
                if(read(events[i].data.fd,&mes,sizeof(message))<0){
                    printf("failed to read mes\n");
                }
                switch (mes.type){
                    case LIST:
                    printf("<Server> Received LIST message from Client[%d]\n",mes.id);
                    printf("<Server> Sending Clients list...\n");
                    build_client_list(clients, mes.message_text, sizeof(clients));
                    if(write(events[i].data.fd,&mes,sizeof(message))<0){
                        printf("Failed to write a LIST message\n");
                        return -1;
                    }
                    break;

                    case S_2ALL:
                    printf("<Server> Received 2ALL message from Client[%d]\n",mes.id);
                    printf("<Server> Sending message to all clients...\n");
                    for (int j = 0; j<MAX_CLIENTS_COUNT;j++){
                        if(clients[j].alive && mes.id != j){
                            if(write(clients[j].fd,&mes,sizeof(message))<0){
                            printf("Failed to write a 2ALL message to clients\n");
                            return -1;
                            }
                        }
                    }
                    if(write(events[i].data.fd,&mes,sizeof(message))<0){
                        printf("Failed to write a 2ALL message\n");
                        return -1;
                    }
                    break;

                    case S_2ONE:
                    printf("<Server> Received 2ONE message from Client[%d]\n",mes.id);
                    printf("<Server> Sending message to Client[%d]...\n",mes.receiver_id);
                    if(clients[mes.receiver_id].alive){
                        if(write(clients[mes.receiver_id].fd,&mes,sizeof(message))<0){
                            printf("Failed to write a 2ONE message to client\n");
                            return -1;
                        }
                    }
                        if(write(events[i].data.fd,&mes,sizeof(message))<0){
                        printf("Failed to write a 2ONE message\n");
                        return -1;
                    }
                    break;

                    case STOP:
                    printf("<Server> Received STOP message from Client[%d]\n",mes.id);
                    printf("<Server> Disconnecting Client...\n");
                    clients[mes.id].alive = false;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clients[mes.id].fd, NULL);
                    if(write(events[i].data.fd,&mes,sizeof(message))<0){
                        printf("Failed to write a STOP message\n");
                        return -1;
                    }
                    break;

                    case ALIVE:
                    printf("received ALIVE message\n");
                    break;

                    default:
                    printf("Not recognized\n");
                    break;
                }
        }
        }
    }

    shoutdown_program(server_fd);
    return 0;
    
}