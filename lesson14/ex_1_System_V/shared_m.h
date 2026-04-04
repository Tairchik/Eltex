#define PROJECTID 2026
#define SEMID 1000
#define PATHNAME "shared_m.h"
#define SIZEBUF 4096

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};

    
struct sembuf lock_1[2] = {{0, 0, 0}, {0, 1, 0}}; 
struct sembuf unlock_1 = {0, -1, 0}; 
struct sembuf lock_2[2] = {{1, 0, 0}, {1, 1, 0}}; 
struct sembuf unlock_2 = {1, -1, 0}; 
