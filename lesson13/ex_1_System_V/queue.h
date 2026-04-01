#define PROJECTID 2026
#define PATHNAME  "queue.h"
#define MSGLEN 10

typedef struct { 
  long type;          
  char data[MSGLEN];   
} msgbuffer;
