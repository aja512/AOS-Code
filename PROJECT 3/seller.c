/*
// COEN 383 AOS Project 3
// File: seller.c
*/

#include <stdio.h>
#include <stdlib.h>

#include "customer.h"
#include "seller.h"

void *seller(void *seller_args) {
    seller_args_t args = *((seller_args_t *) seller_args);

    printf("{%s} id: %d, priority: %d\n", args.name, args.id, args.priority);

    return NULL;
}
