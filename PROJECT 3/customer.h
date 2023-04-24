#ifndef CUSTOMER_H
#define CUSTOMER_H

#define MAX_ARRIVAL_TIME 100

typedef enum {
    HIGH_PRIORITY,
    MEDIUM_PRIORITY,
    LOW_PRIORITY
} priority_t;

typedef struct {
    char name[5];
    int arrival_time;
} customer_t;

typedef struct {
    size_t size;
    customer_t *buf;
} customer_queue_t;

customer_queue_t *create_customer_queue(int num_customers);
void print_customer_queue(customer_queue_t *queue);

#endif
