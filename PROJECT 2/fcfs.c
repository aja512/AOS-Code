/*
//    COEN 383 Project 2
//    Group: 1
//    File: fcfs.c
*/

#include "common.h"
/*
/ next_prc_fcfs:
// This function returns a pointer to a valid process that should be process next.
//
//  Inputs:
//  ptr - pointer to the array of procs
//  Q - the current quanta to compare against arrival time
//  c - pointer to the last proc that had the cpu
//  nop - not used in this function
*/

process * next_proc_fcfs(process* ptr, int Q, process* c, int nop){
    int i =0;
    if (c != NULL && c->remaining_runtime > 0) return c; // non-preemptive-ness
    for (i=0; i< NUMBER_OF_PROCS; i++){
        if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0) return &ptr[i];
    }
    return NULL;
}