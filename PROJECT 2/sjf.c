/*
//    COEN 383 Project 2
//    Group: 1
//    File: sjf.c
*/

#include "common.h"
/*
// next_prc_sjf:
// This function returns a pointer to a valid process that should be processed next.
//
//  Inputs:
//  ptr - pointer to the array of procs
//  Q - the current quanta to compare against arrival time
//  S - flag to ask for processes that have already started only
//  c - pointer to the last proc that had the cpu
*/

process * next_proc_sjf(process* ptr, int Q, int S, process* c){
    int i =0;
    if (c != NULL && c->remaining_runtime > 0 && c->remaining_runtime != c->expected_runtime) return c; // non-preemptive-ness
    for (i=0; i< NUMBER_OF_PROCS; i++){
        if(S){
            if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0 && ptr[i].remaining_runtime != ptr[i].expected_runtime) return &ptr[i];
        }else{
            if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0) return &ptr[i];
        }
    }
    return NULL;
}