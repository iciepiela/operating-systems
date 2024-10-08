#define _XOPEN_SOURCE 700


#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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

message_type to_message_type(const char *str)
{
    if (strcmp(str, "LIST") == 0)
        return LIST;
    if (strcmp(str, "2ALL") == 0)
        return S_2ALL;
    if (strcmp(str, "2ONE") == 0)
        return S_2ONE;
    if (strcmp(str, "STOP") == 0)
        return STOP;

    return -1;
}


int main(int argc, char* argv[]) {

    if (argc < 4) {
        printf("Wrong number of arguments\n");
    }

    char* client_name = argv[1];
    char* ipv4_address = argv[2];
    int id;
    uint16_t port = (uint16_t)strtol(argv[3],NULL,10);
    struct epoll_event events[MAX_CLIENTS_COUNT],new_event;



    int client_fd;
    struct sockaddr_in serv_addr;
    message mes;


    client_fd = socket(AF_INET,SOCK_STREAM,0);

    if(client_fd < 0){
        printf("Failed to create a socket\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET,ipv4_address,&serv_addr.sin_addr)<=0){
        printf("Invalid address\n");
        return -1;
    }

    if (connect(client_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
        printf("Failed to connect\n");
        return -1;
    }

    mes.type = INIT;
    strncpy(mes.message_text, client_name, MAX_NAME_LEN - 1);
    printf("<Client %s> Sending init to server\n",client_name);

    if(write(client_fd,&mes,sizeof(message))<0){
        printf("Sending init message failed\n");
        return -1;
    }

    if(read(client_fd,&mes,sizeof(message))<0){
        printf("Reading init message failed\n");
        return -1;
    }

    id = mes.id;


    if (fork() == 0){
        int epoll_fd = epoll_create1(0);
        if (epoll_fd<0){
            printf("Failed to create epoll_fd\n");
            return -1;
        }
        new_event.data.fd = client_fd;
        new_event.events = EPOLLIN;

        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_event.data.fd, &new_event)<0)
            {
                printf("Failed to add server_fd to epoll\n");
                close(epoll_fd);
                return -1;
            }

        while (server_running){
        message received_message;
        int event_count;
        event_count = epoll_wait(epoll_fd,events,MAX_CLIENTS_COUNT,-1);
        if (event_count<0){
            printf("Failed to epoll_wait\n");
            return -1;
        }
        for (int i = 0;i<event_count;i++){

            if(events[i].events && EPOLLIN){
                if(read(events[i].data.fd,&received_message,sizeof(message))<0){
                    printf("failed to read mes\n");
                }
                switch(received_message.type){
                    case LIST:

                    printf("<Client %s> Received a list of clients from the server\n",client_name);
                    printf("%s\n",received_message.message_text);
                    
                    break;

                    case S_2ALL:
                    if(received_message.id==id){
                        printf("<Client %s> Message sent successfully!\n",client_name);
                    }
                    else{
                        printf("<Client %d> %s\n",received_message.id,received_message.message_text);
                    }
                    break;

                    case S_2ONE:
                    if(received_message.id==id){
                        printf("<Client %s> Message sent successfully!\n",client_name);
                    }
                    else{
                        printf("<Client %d> %s\n",received_message.id,received_message.message_text);
                    }

                    break;

                    case STOP:
                    printf("<Client %s> BYE BYE!\n",client_name);
                    server_running = false;
                    
                    break;    

                    case ALIVE:
                    write(client_fd, &received_message, sizeof(message));
                    break;
                    
                default:
                break;

                }
            }
        }
        }
        close(epoll_fd);
        return 0;
    

    }

    printf("<Client %s> New Id %d received\n",client_name,id);
    char *buffer;
    while(server_running && scanf("%ms",&buffer)==1){
        char command[4];
        strncpy(command, buffer, 4);
        message mes;
        message_type message_type = to_message_type(command);


        mes.type = message_type;
        mes.id = id;

        switch (message_type) {
            case LIST:
                printf("<Client %s> Sending LIST message to server...\n", client_name);
                if (write(client_fd, &mes, sizeof(message)) < 0) {
                    printf("Failed to write LIST message\n");
                    return -1;
                }
                break;

            case S_2ALL:
                scanf("%s", mes.message_text);
                printf("<Client %s> Sending 2ALL message to server...\n", client_name);
                if (write(client_fd, &mes, sizeof(message)) < 0) {
                    printf("Failed to write 2ALL message\n");
                    return -1;
                }
                break;

            case S_2ONE:
                scanf("%s", mes.message_text);
                scanf("%d",&mes.receiver_id);
                printf("<Client %s> Sending 2ONE message to server...\n",client_name);
                if(write(client_fd,&mes,sizeof(message))<0){
                    printf("Failed to write 2ONE message\n");
                    return -1;
                    }
                break;
            case STOP:
                printf("<Client %s> Sending STOP message to server...\n",client_name);
                if(write(client_fd,&mes,sizeof(message))<0){
                    printf("Failed to write STOP message\n");
                    return -1;
                    }
                break;
            default:
            break;
            }

        }


    shutdown(client_fd,SHUT_RDWR);
    close(client_fd);
    
    return 0;
}

