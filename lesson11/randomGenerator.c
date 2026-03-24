#include "randomGenerator.h"
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


int srandInit = 0;


int generateStartStoreProduct()
{
    if (srandInit == 0)
    {
        srand(time(NULL));
        srandInit = 1;
    }
    
    return 900 + rand() % 201;
}

void fillOutStores(int* stores, int num_stores)
{
    for (int i = 0; i < num_stores; i++)
    {
        stores[i] = generateStartStoreProduct();
    }
}

int generateCustomerProduct()
{
    if (srandInit == 0)
    {
        srand(time(NULL));
        srandInit = 1;
    }
    
    return 9900 + rand() % 201;
}

pthread_mutex_t rand_mutex = PTHREAD_MUTEX_INITIALIZER;

int getRand(int start, int end)
{
    pthread_mutex_lock(&rand_mutex);
    int result = start + rand() % (end - start + 1);
    pthread_mutex_unlock(&rand_mutex);
    
    return result;
}
