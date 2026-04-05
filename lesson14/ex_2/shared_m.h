#define PATHNAME "/shared_m.h"
#define SEMREAD "/sem1"
#define SEMWRITE  "/sem2"
#define SEMCOUNTCLIENTS "/sem3"

#define CLIENTS 100
#define MAX_MSG 1000
#define MAX_TEXT 256
#define MAX_NAME 32
#define LEN_MSG MAX_TEXT + MAX_NAME + 4 + 1

#define MSG_LOGIN 1
#define MSG_TEXT  2
#define MSG_SERVER 3

#define BUFSIZE sizeof(Message) * MAX_MSG


typedef struct{
    char name[MAX_NAME];
    char text[MAX_TEXT];
    unsigned char status;
    int pid_client;
}Message;
