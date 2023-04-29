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


// Structure for defining thr seat status, seller info, and customer id.
typedef struct 
{
    int occupied;
    char seller_type;
    int seller_id;
    int customer_id;
} chart_struct;

// Function for printing the seating arrangement
void print_seat_chart(chart_struct chart[][10]) 
{
    printf("Seating chart: \n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (chart[i][j].occupied == 0)
                printf("- ");
            else
                printf("%c%d%d ", chart[i][j].seller_type, chart[i][j].seller_id, chart[i][j].customer_id);   // print each seat in matrix chart
        }
        printf("\n");
    }
    printf("\n");
}

// Main Function
int main(int argc, char *argv[]) 
{   
    srand(time(NULL));

    int N = atoi(argv[1]);  // argv[1]s has number customers each seller expects in an hour

    seller_args_t args[10];
    pthread_t threads[10];

    chart_struct seats[10][10];     // seats matrix
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            seats[i][j].occupied = 0;   // initialize seats matrix to set all seats empty
        }
    }
    print_seat_chart(seats);

    for (int i = 0; i < 10; i++)    //Initialize arguments for each seller queue first
    {
        if (i == 0) 
        {
            // create customer queues for high-price seller
            customer_queue_t *queue = create_customer_queue(N);
            
            // print queue
            printf("Initial H queue:  ");
            print_customer_queue(queue);

            args[i] = (seller_args_t) {queue, HIGH_PRIORITY, {0}, i };
            sprintf(args[i].name, "H");
        } 
        else if (i > 0 && i < 4) 
        {
            // create customer queues for med-price sellers
            customer_queue_t *queue = create_customer_queue(N);

            // print queue
            printf("Initial M%d queue: ", i);
            print_customer_queue(queue);

            args[i] = (seller_args_t) {queue, MEDIUM_PRIORITY, {0}, i };
            sprintf(args[i].name, "M%d", i);
        } 
        else 
        {
            // create customer queues for low-price seller
            customer_queue_t *queue = create_customer_queue(N);

            // print queue
            printf("Initial L%d queue: ", i-3);
            print_customer_queue(queue);

            args[i] = (seller_args_t) {queue, LOW_PRIORITY, {0}, i };
            sprintf(args[i].name, "L%d", i - 3);
        }
    }
    printf("\n");

    // initialize thread for all sellers
    for (int i = 0; i < 10; i++) {
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
