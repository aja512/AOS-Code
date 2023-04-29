/*
// Header file assigning priority and defining process id, arrival time, and queue size.
*/

#ifndef BUYER_H
#define BUYER_H

#define MAX_ARRIVAL_TIME 60

typedef enum 
{
    HIGH_PRIORITY,
    MEDIUM_PRIORITY,
    LOW_PRIORITY
} priority_t;

typedef struct 
{
    int id;
    int arrival_time;
} customer_t;

typedef struct 
{
    size_t size;
    customer_t *buf;
} customer_queue_t;

customer_queue_t *create_customer_queue(int num_customers);
void print_customer_queue(customer_queue_t *queue);

customer_t dequeue_customer(customer_queue_t *queue);
void print_customer(customer_t customer);

#endif
