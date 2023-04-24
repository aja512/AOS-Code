/*
Header files for handling events: Arrival, Inform, Leaving
*/

#ifndef COEN383_P3_EVENT_H
#define COEN383_P3_EVENT_H

typedef enum {
    Arrived,
    Informed,
    Left
} Event;

typedef struct {
    Event event;
    int seat_num;
    int time_stamp;
} event_t;


typedef struct event_pool event_pool_t;

event_pool_t* event_pool_init();
void event_pool_destroy(event_pool_t* event_pool);
void add_event(event_pool_t* event_pool, event_t event);
void print_events(event_pool_t* event_pool);
void event_print(event_t event);
#endif 
