#ifndef SELLER_H
#define SELLER_H

typedef struct {
    customer_t *queue;
    priority_t priority;
    char name[3];
    int id;
} seller_args_t;

void *seller(void *args);

#endif
