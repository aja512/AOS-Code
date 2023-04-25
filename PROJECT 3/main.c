/*
// COEN 383 AOS Project 3
// File: main.c
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "buyer.h"
#include "seller.h"

int main(int argc, char *argv[]) 
{
    srand(time(NULL));
    
    customer_queue_t *queue = create_customer_queue(10);
    print_customer_queue(queue);


    seller_args_t args[10];
    pthread_t threads[10];

    for (int i = 0; i < 10; i++) 
    {
        if (i == 0) 
        {
            args[i] = (seller_args_t) { NULL, HIGH_PRIORITY, {0}, i };
            sprintf(args[i].name, "H");
        } 
        else if (i > 0 && i < 4) 
        {
            args[i] = (seller_args_t) { NULL, MEDIUM_PRIORITY, {0}, i };
            sprintf(args[i].name, "M%d", i);
        } 
        else 
        {
            args[i] = (seller_args_t) { NULL, LOW_PRIORITY, {0}, i };
            sprintf(args[i].name, "L%d", i - 3);
        }

        if (pthread_create(&threads[i], NULL, seller, &args[i])) 
        {
            printf("\n Thread Creation Error!!! \n");
        }
    }

    // wait for all seller threads to exit  
    for (int i = 0; i < 10; i++) 
    {
        if (pthread_join(threads[i], NULL)) 
        {
            printf("\n Thread is not joining. Error!!! \n");
        }
    }
}
