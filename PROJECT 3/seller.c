/*
// COEN 383 AOS Project 3
// File: seller.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "buyer.h"
#include "seller.h"

void *seller(void *seller_args) {
    seller_args_t args = *((seller_args_t *) seller_args);
    printf("{%s} id: %d, priority: %d\n", args.name, args.id, args.priority);

    // initialize seller's internal clock
    int sim_hour = 0;       // simulation clock hour value, 0
    int sim_tenmin = 0;     // simulation clock ten minutes value, 0-5
    int sim_onemin = 0;     // simulation clock one minute value, 0-9

    // Continue for the full hour
    for (int i=0; i<60; i++)
    {
        if(args.queue->buf[0].arrival_time <= i)    // handle customer that has arrived at front of queue
        {
            //printf("Time is %d ", i);
            customer_t current = dequeue_customer(args.queue);
            printf("[%d:%d%d]", sim_hour, sim_tenmin, sim_onemin);
            print_customer(current);
        }
        if (args.queue->size == 0) break;   // end early if seller finishes handling all customers

        // update timestamp
        sim_onemin++;
        if (sim_onemin >= 10)
        {
            sim_tenmin = sim_tenmin + (sim_onemin/10);
            sim_onemin = sim_onemin % 10;
            if (sim_tenmin >= 10)
            {
                sim_tenmin = sim_tenmin % 10;
                sim_hour++;
            }
        }
    }
    return NULL;
}
