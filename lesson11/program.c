#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "randomGenerator.h"

#define NUM_STORES 5 
#define NUM_CUSTOMER 3
#define DELIVERY_PRODUCTS 200

int stores[NUM_STORES];
int num_of_working_threads = NUM_CUSTOMER;

pthread_mutex_t stores_m[NUM_STORES] = {
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_MUTEX_INITIALIZER
};

pthread_mutex_t leave_thread_m = PTHREAD_MUTEX_INITIALIZER;

typedef struct Customer
{
    pthread_t thread;
    int tid;
    int products; 
}Customer;

void* customer_buy(void* args)
{
    Customer* customer = (Customer*) args;
    int store = 0;
    printf("Покупатель %d начал работу, начальное количество товаров %d\n", customer->tid, customer->products);
    while (1)
    {
        store = getRand(0, 4);
        pthread_mutex_lock(&stores_m[store]);
        printf("Покупатель %d зашел в ларек %d, количество товаров %d\n", customer->tid, store, stores[store]);
        customer->products -= stores[store];
        stores[store] = 0;
        printf("У покупателя %d стало %d продукций\n", customer->tid, customer->products);
        pthread_mutex_unlock(&stores_m[store]);

        if (customer->products <= 0)
        {
            pthread_mutex_lock(&leave_thread_m);
            num_of_working_threads -= 1;
            pthread_mutex_unlock(&leave_thread_m);
            printf("Покупатель %d завершил работу\n", customer->tid);
            return NULL;
        }                

        sleep(2);
    }
}


void* delivery_store(void* args)
{
    int store = 0;
    while (1)
    {
        store = getRand(0, 4);
        pthread_mutex_lock(&stores_m[store]);
        printf("Погрузчик зашел в ларек %d, количество товаров %d\n", store, stores[store]);
        stores[store] += 200;
        printf("Погрузчик пополнил запасы в ларьке %d, количество товаров %d\n", store, stores[store]);
        pthread_mutex_unlock(&stores_m[store]);

        pthread_mutex_lock(&leave_thread_m);
        if (num_of_working_threads == 0)
        {
            printf("Погрузчик завершил работу\n");
            pthread_mutex_unlock(&leave_thread_m);
            return NULL;
        }
        pthread_mutex_unlock(&leave_thread_m);

   
        sleep(1);

    }
    return NULL;
}

int main()
{
    Customer customers[NUM_CUSTOMER];
    pthread_t delivery;
    int *s;
    // Заполняем начальные продукты в магазине и в покупателях
    fillOutStores(stores, NUM_STORES);
    for (int i = 0; i < NUM_CUSTOMER; i++)
    {
        customers[i].products = generateCustomerProduct();
    }
    
    for (int i = 0; i < NUM_CUSTOMER; i++)
    {
        customers[i].tid = i;
        pthread_create(&customers[i].thread, NULL, customer_buy, (void *)&customers[i]);        
    }
    
    pthread_create(&delivery, NULL, delivery_store, NULL);        

    for (int i = 0; i < NUM_CUSTOMER; i++)
    {
        pthread_join(customers[i].thread, (void **) &s);    
    }
    pthread_join(delivery, (void **) &s);    


    exit(EXIT_SUCCESS);    
}