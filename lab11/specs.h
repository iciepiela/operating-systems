#define MAX_CLIENTS_COUNT 4
#define MAX_NAME_LEN 20
#define MAX_MESS_LEN 1000



typedef enum message_type{
    INIT,
    LIST,
    S_2ALL,
    S_2ONE,
    STOP,
    ALIVE

}message_type;

typedef struct message {
    int id;
    int receiver_id;
    message_type type;
    char message_text[MAX_MESS_LEN];
} message;

typedef struct client{
    bool alive;
    char name[MAX_NAME_LEN];
    int id;
    int fd;
}client;