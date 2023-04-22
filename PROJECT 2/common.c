/*
//    COEN 383 - Operating Systems Project 2
//    Group: 1 
//    File: common.c
//
//    common functions used across the alggorithms
//
*/

#include "common.h"

// Function to compare the arrival time of a process
int compare_arrival_times(const void * a, const void * b)
{
    process *p1 = (process*)a;
    process *p2 = (process*)b;
    float r = p1->arrival_time - p2->arrival_time;
    if (r>0) return 1;
    if (r<0) return -1;
    return r;
}

// Function to compare the expected runtimes of a process
int compare_expected_runtimes(const void * a, const void * b)
{
    process *p1 = (process*)a;
    process *p2 = (process*)b;
    float r = p1->expected_runtime - p2->expected_runtime;
    if (r>0) return 1;
    if (r<0) return -1;
    return r;
}

// Function to compare the remaining time of a process
int compare_remaining_runtimes(const void * a, const void * b)
{
    process *p1 = (process*)a;
    process *p2 = (process*)b;
    float r = p1->remaining_runtime - p2->remaining_runtime;
    if (r>0) return 1;
    if (r<0) return -1;
    return r;
}

// Function to compare the priorities of a process
int compare_priorities(const void * a, const void * b)
{
    process *p1 = (process*)a;
    process *p2 = (process*)b;
    return (p1->priority - p2->priority);
}

// Function to generate processes with pseudo random parameters
// Takes a pointer to a pre-allocated buffer and populates it.
int generate_procs(process *p)
{
    int pr;
    int i;
    float tmp;
    for(i=0; i<NUMBER_OF_PROCS; i++)
    {
        p[i].pid = i;

        tmp = rand() % 1000;
        tmp = tmp / 10;
        p[i].arrival_time = tmp;

        tmp = rand() % 101;
        tmp = tmp / 10;
        if (tmp < 1) tmp += 1;
        p[i].expected_runtime = tmp;
        p[i].remaining_runtime = tmp;

        pr = rand() % 5;
        if (pr == 0) pr += 1;
        p[i].priority = pr;
    }

    return 0;
}

// Function to print the processes in a buffer.
int print_procs(process *b){
    int i;
    printf("________________________________________________________\n");
    printf("|   pid    |   a time |   e time |   r time |  priority|\n");
    printf("|======================================================|\n");
    for (i=0; i<NUMBER_OF_PROCS; i++){

        printf("|   %-7d|   %-7.1f|   %-7.1f|   %-7.1f|   %-7d|\n", \
            b[i].pid,\
            b[i].arrival_time,\
            b[i].expected_runtime,\
            b[i].remaining_runtime,\
            b[i].priority);

    }
    printf("========================================================\n");

    return 0;
}
