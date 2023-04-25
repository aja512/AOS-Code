/*
// COEN 383 AOS Project 3
// File: buyer.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "buyer.h"

int customer_comparator(const void *a, const void *b) 
{
    customer_t *cust_a = (customer_t *) a;
    customer_t *cust_b = (customer_t *) b;

    return cust_a->arrival_time - cust_b->arrival_time;
}

customer_queue_t *create_customer_queue(int num_customers) 
{
    size_t customer_t_sz = sizeof(customer_t);

    customer_queue_t *queue = malloc(sizeof(customer_queue_t));

    queue->size = num_customers;
    queue->buf = calloc(num_customers, customer_t_sz);

    for (int i = 0; i < num_customers; i++) 
    {
        queue->buf[i].arrival_time = rand() % MAX_ARRIVAL_TIME;
    }

    qsort(queue->buf, num_customers, customer_t_sz, customer_comparator);

    return queue;
}

void print_customer_queue(customer_queue_t *queue) 
{
    printf("[ ");
    for (int i = 0; i < queue->size; i++) 
    {
        printf("{at: %d} ", queue->buf[i].arrival_time);
    }
    printf("]\n");
}
