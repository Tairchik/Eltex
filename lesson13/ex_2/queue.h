#define PATHNAME_LOGIN "queue.h"
#define PATHNAME_SEND_MESSAGES "server.c"
#define PATHNAME_LOGIN_ID 1022
#define MAX_TEXT 256
#define MAX_NAME 32
#define LEN_MSG MAX_TEXT + MAX_NAME + 4
#define MSG_LOGIN 1
#define MSG_TEXT  2
#define MSG_SERVER 3

typedef struct{
    long type;
    char name[MAX_NAME];
    char text[MAX_TEXT];
    int pid_client;
}Message;
