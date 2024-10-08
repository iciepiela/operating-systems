#define SERVER_QUEUE_NAME "/server_queue_name"
#define MAX_CLIENTS_COUNT 4

typedef enum {
    INIT,
    MESSAGE,
    NONE
} message_type;

typedef struct {
    int id;
    message_type type;
    char message_text[2048];
} message;