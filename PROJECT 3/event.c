/*
// COEN 383 AOS Project 3
// File: event.c
*/

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "event.h"

// Structure for defining the event (Linked List)
struct node 
{
    event_t event;
    struct node *next;
};

// Defining Event Pool 
struct event_pool 
{
    struct node *dummy_head;
    struct node *new_event_entry;
    pthread_mutex_t new_event_mutex;


    struct node *printer_look;
    int printer_job_remaining;
    pthread_mutex_t job_remaining_mutex;
    pthread_cond_t printer_signal;
    pthread_t printer_pid;
    bool printer_stop;
};

// Print the events
void event_print(event_t event) 
{

}

// Print the event pool in real-time
void* real_time_printer(void *event_pool) 
{
    struct event_pool *pool = (struct event_pool*)event_pool;
  
  
    while (!pool->printer_stop) 
    {
        int job_number;
        pthread_mutex_lock(&(pool->job_remaining_mutex));
      
      
        while (pool->printer_job_remaining == 0) 
        {
            if (pool->printer_stop) 
            {
                return NULL;
            }
            pthread_cond_wait(&(pool->printer_signal), &(pool->job_remaining_mutex));
        }
        job_number = pool->printer_job_remaining;
        pool->printer_job_remaining = 0;
        pthread_mutex_unlock(&(pool->job_remaining_mutex));
      
      
      
        while (job_number > 0) 
        {
            pool->printer_look = pool->printer_look->next;
            event_print(pool->printer_look->event);

            job_number -= 1;
        }
    }
    return NULL;
}

// Destroying the event pool

void event_pool_destroy(event_pool_t *event_pool) 
{
    // real time printer
    event_pool->printer_stop = true;
    pthread_cond_signal(&(event_pool->printer_signal));
    pthread_join(event_pool->printer_pid, NULL);
    pthread_cond_destroy(&(event_pool->printer_signal));
    pthread_mutex_destroy(&(event_pool->job_remaining_mutex));



    struct node *op = event_pool->dummy_head;
  
  
    while (op != NULL) 
    {
        struct node *delete = op;
        op = op->next;
        free(delete);
    }
    pthread_mutex_destroy(&(event_pool->new_event_mutex));

    free(event_pool);

}

// Creating Event Pool

event_pool_t *event_pool_init() 
{
    struct event_pool *new = malloc(sizeof(struct event_pool));
    new->dummy_head = malloc(sizeof(struct node));
    new->dummy_head->next = NULL;
    new->new_event_entry = new->dummy_head;
    pthread_mutex_init(&(new->new_event_mutex), NULL);

    //real time printer
    pthread_mutex_init(&(new->job_remaining_mutex), NULL);
    pthread_cond_init(&(new->printer_signal), NULL);
    new->printer_job_remaining = 0;
    new->printer_stop = false;
    new->printer_look = new->dummy_head;
    pthread_create(&(new->printer_pid), NULL, real_time_printer, new);
    return new;
}

// Function for adding events into the process pool
void add_event(event_pool_t *event_pool, event_t event) 
{
    struct node *new_node = malloc(sizeof(struct node));
    new_node->next = NULL;
    new_node->event = event;
    pthread_mutex_lock(&(event_pool->new_event_mutex));
    event_pool->new_event_entry->next = new_node;
    event_pool->new_event_entry = new_node;
    pthread_mutex_unlock(&(event_pool->new_event_mutex));

    // real time printer
    pthread_mutex_lock(&(event_pool->job_remaining_mutex));
    event_pool->printer_job_remaining += 1;
    pthread_mutex_unlock(&(event_pool->job_remaining_mutex));
    pthread_cond_signal(&(event_pool)->printer_signal);

}
